#include <iomanip>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "net.h"
#include "lidar.h"
#include "audio.h"
#include "usfs_master.h"

namespace logging = boost::log;
namespace keywords = boost::log::keywords;

// --- Create video capture object and global variables ---
cv::VideoCapture cap(gstreamer_pipeline(1280, 720, 1280, 720, 10, 0), cv::CAP_GSTREAMER); //gstreamer_pipeline(1280, 720, 1280, 720, 10, 0), cv::CAP_GSTREAMER
cv::Mat stream;
bool stop = false;
bool standby = false;
enum class Imclass {Day, Night, None};

std::vector<std::array<int, 4>> warnings;
AudioPlayer* player = new AudioPlayer();
LidarLite_v3* lidar = new LidarLite_v3();
int distance_mean = 1000;

static const uint8_t  MAG_RATE       = 100;  // Hz
static const uint16_t ACCEL_RATE     = 200;  // Hz
static const uint16_t GYRO_RATE      = 200;  // Hz
static const uint8_t  BARO_RATE      = 50;   // Hz
static const uint8_t  Q_RATE_DIVISOR = 3;    // 1/3 gyro rate

// --- Define functions ---
void get_frame() {
    BOOST_LOG_TRIVIAL(info) << "Starting video thread...";
    while (!stop) { // Get constant video stream using separate thread
        if (!standby) {
            cap >> stream;
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Received stop command; Stopping video thread.";
}

void measure_distance() {
    BOOST_LOG_TRIVIAL(info) << "Starting distance thread...";
    std::vector<float> dist_vec;
    float dist;
    __u8  busyFlag;
    unsigned int counter, divider = 0;
    while (!stop) {
        if (!standby) {
            busyFlag = lidar->getBusyFlag();
            if (busyFlag == 0x00) {
                lidar->takeRange();
                dist = static_cast<float>(lidar->readDistance());
            
                if (dist <= 1)
                    dist = 1000;
                
                dist_vec.push_back(dist);
                if (dist_vec.size() > 10)
                    dist_vec.erase(dist_vec.begin());
            }
            
            counter += 1;
            int dist_mean = static_cast<int>(mean(dist_vec));
            distance_mean = dist_mean;
        
            if (dist_mean < 300 && dist_mean > 225)
                divider = 540;
            if (dist_mean < 225 && dist_mean > 150)
                divider = 380;
            if (dist_mean < 150 && dist_mean > 75)
                divider = 220;
            if (dist_mean < 75)
                divider = 1;
            if (dist_mean > 300) {
                divider = 0;
            }
        
            if (counter % divider == 0 && divider != 0) {
                if (!player->is_playing(1)) {
                    if (divider == 1)
                        player->play_sample(LONG_BEEP, 1);
                    else
                        player->play_sample(SHORT_BEEP, 1);
                }
            }
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        }
    }
    BOOST_LOG_TRIVIAL(info) << "Received stop command; Stopping distance thread.";
    dist_vec.clear();
}

Imclass get_image_class(cv::Mat *img) {
    cv::Mat dst, hsv;
    int sum_val = 0, mean_val = 0;
    
    cv::resize(*img, dst, cv::Size(64, 35));
    cv::cvtColor(dst, hsv, cv::COLOR_BGR2HSV);
    for (int i = 0; i <= 15; i++) {
        for (int j = 0; j < hsv.cols; j++) {
            cv::Vec3b pixel = hsv.at<cv::Vec3b>(i, j);
            sum_val += static_cast<int>(pixel.val[2]);
        }
    }
    mean_val = sum_val / (16 * hsv.cols);
    
    if (mean_val < 90) 
        return Imclass::Night;
    else
        return Imclass::Day;
}

float laplacian(cv::Mat *fr) {
    cv::Scalar m, stdv;
    cv::Mat gray, lap;

    cv::cvtColor(*fr, gray, cv::COLOR_BGR2GRAY);
    cv::Laplacian(gray, lap, CV_64F);
    cv::meanStdDev(lap, m, stdv, cv::Mat());

    return static_cast<float>(std::pow(stdv.val[0], 2));
}

void process_warnings() {
    int max_val = 0, max = -1, del = -1;

    for (std::size_t i = 0; i < warnings.size(); i++) {
        if (warnings[i][3] > max_val && warnings[i][1] == 0) { // Find warning with highest priority
            max_val = warnings[i][3];
            max = i;
        }
        if (warnings[i][1] == 1) // Iterate counter
            warnings[i][2] += 1;
        if (warnings[i][2] > 10) // When counter full, mark as 'to be removed'
            del = i;
    }

    if (max != -1) { // Play warning and mark as played
        if (!player->is_playing(0)) {
            player->play_sample(warnings[max][0], 0);
            warnings[max][1] = 1;
        }
    }

    if (del != -1) // Remove warning from vector
        warnings.erase(warnings.begin() + del);
}

void init_logging() {
    logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");
    logging::add_file_log(keywords::file_name = "log/runtime.log", keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%");
    logging::add_console_log(std::cout, keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%");
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
    logging::add_common_attributes();
}

// --- MAIN FUNCTION ---
int main(int argc, char** argv) {
    init_logging();
    player->set_volume(0, MIX_MAX_VOLUME);
    player->set_volume(1, MIX_MAX_VOLUME);
    
    // --- Play startup sequence ---
    player->play_sample(STARTUP_SEQUENCE, 0);
    std::this_thread::sleep_for(std::chrono::seconds(13));
    
    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    
    // --- Define variables and create objects ---
    std::string modelConfiguration = "model/MobileNetSSDV2_deploy.prototxt";
    std::string modelBinary = "model/MobileNetSSDV2.caffemodel";
    bool quit = false;
    
    Network* mobilenet = new Network(modelConfiguration, modelBinary);
    mobilenet->initialize();
    cv::Mat detections;
    int trafficlight_switch = -1, trafficlight_counter = 0;
    Imclass image_class_prev = Imclass::Day, image_class_edge = Imclass::None;
    unsigned int image_class_counter = 0;
    float lap = 0.0f;
    
    USFS* motion_sen = new USFS(MAG_RATE, ACCEL_RATE, GYRO_RATE, BARO_RATE, Q_RATE_DIVISOR);
    std::vector<float> az_list;
    std::vector<float> alt_list_prev;
    std::vector<float> alt_list_new;
    bool register_gesture = false;
    bool moving = true;
    bool on_slope = false;
    int shutdown_counter = -1;
    
    // --- Terminate program if encountering an error
    bool lidar_state = lidar->i2c_init(), usfs_state = motion_sen->begin(0), cap_state = cap.isOpened();
    if (!lidar_state || !usfs_state || !cap_state) {
        if (!lidar_state)
            BOOST_LOG_TRIVIAL(error) << "Failed initializing lidar sensor; Aborting.";
        if (!usfs_state) {
            BOOST_LOG_TRIVIAL(error) << motion_sen->getErrorString();
            BOOST_LOG_TRIVIAL(error) << "Failed initializing motion sensor; Aborting.";
        }
        if (!cap_state)
            BOOST_LOG_TRIVIAL(error) << "Failed to open video source; Aborting.";
        
        stop = true;
        cap.release();
        
        delete mobilenet;       // Delete MobileNet
        mobilenet = nullptr;
        delete motion_sen;      // Delete motion sensor
        motion_sen = nullptr;
        delete lidar;           // Delete lidar sensor
        lidar = nullptr;
        delete player;          // Delete audio player
        player = nullptr;
        
        system("sudo /bin/sh -c shutdown -h now");
        return -1;
    }
    cap >> stream;
    std::thread videoThread(get_frame);
    
    // --- Play startup warning message
    player->play_sample(STARTUP_WARNING, 0);
    std::this_thread::sleep_for(std::chrono::seconds(15));
    
    // --- Play start signal
    player->play_sample(SIGN_START, 0);
    std::thread distanceThread(measure_distance);
    
    // --- MAIN LOOP ---
    for (;;) {
        cv::Mat frame = stream;
        
        // --- Check for errors
        if (frame.empty()) {
            BOOST_LOG_TRIVIAL(error) << "Failed to get frame image from video source; Aborting.";
            player->play_sample(ERROR_CAM, 0);
            std::this_thread::sleep_for(std::chrono::seconds(6));
            break;
        }
        motion_sen->checkEventStatus();            
        if (motion_sen->gotError()) {
            BOOST_LOG_TRIVIAL(error) << motion_sen->getErrorString();
            BOOST_LOG_TRIVIAL(error) << "Above error occured whilst checking motion sensor status; Aborting.";
            player->play_sample(ERROR_USFS, 0);
            std::this_thread::sleep_for(std::chrono::seconds(7));
            break;
        }
        
        // --- PHASE 1: Collect data ---
        float roll, pitch, yaw;
        motion_sen->Quaternion(roll, pitch, yaw);
        float ax, ay, az;
        motion_sen->Accelerometer(ax, ay, az);
        float gx, gy, gz;
        motion_sen->Gyroscope(gx, gy, gz);
        float mx, my, mz;
        motion_sen->Magnetometer(mx, my, mz);
        float temperature, pressure, altitude;
        motion_sen->Barometer(pressure, temperature, altitude);
        
        // --- PHASE 2: Process data ---
        
        // --- Recognize gestures ---
        if (pitch > 60.0f && !standby) { // Tilt to right
            if (register_gesture) {
                time_t now = time(0);
                tm* ltm = localtime(&now);
                int hour = static_cast<int>(ltm->tm_hour);
                int temp = (int)temperature;
                
                hour = clamp(hour, 13, 18);
                temp = clamp(temp, 10, 35);
                if (temp % 5 != 0)
                    temp = temp + 5 - (temp % 5);
                
                if (!vector_contains(warnings, hour + 5))
                    warnings.push_back({hour + 5, 0, 0, 112});
                if (!vector_contains(warnings, (temp / 5) + 22))
                    warnings.push_back({(temp / 5) + 22, 0, 0, 111});
                if (pressure > 800.0f) {
                    if (!vector_contains(warnings, RAIN_NO))
                        warnings.push_back({RAIN_NO, 0, 0, 110});
                } else {
                    if (!vector_contains(warnings, RAIN_YES))
                        warnings.push_back({RAIN_YES, 0, 0, 110});
                }
                
                register_gesture = false;
            }
        } else if (pitch < -60.0f) { // Tilt to left
            if (register_gesture) {
                if (standby) {
                    standby = false;
                    mobilenet = new Network(modelConfiguration, modelBinary);
                    mobilenet->initialize();
                    if (!vector_contains(warnings, STANDBY_OFF))
                        warnings.push_back({STANDBY_OFF, 0, 0, 900});
                } else {
                    standby = true;
                    mobilenet = nullptr;
                    
                    if (!vector_contains(warnings, STANDBY_ON))
                        warnings.push_back({STANDBY_ON, 0, 0, 910});
                }
                register_gesture = false;
            }
        } else if (!standby && roll < -140.0f && pitch < 30.0f && pitch > -30.0f) { // Tilt down
            if (register_gesture && !vector_contains(warnings, FALLING)) {
                warnings.push_back({FALLING, 0, 0, 610});
                register_gesture = false;
            }
        } else if (!standby && roll > -10.0f && pitch < 30.0f && pitch > -30.0f) { // Tilt up
            if (register_gesture) {
                if (shutdown_counter == -1) {
                    if (!vector_contains(warnings, SHUTDOWN_CONF))
                        warnings.push_back({SHUTDOWN_CONF, 0, 0, 999});
                    shutdown_counter = 600;
                } else {
                    quit = true;
                }
                register_gesture = false;
            }
        } else
            register_gesture = true;
            
        if (shutdown_counter != -1)
            shutdown_counter -= 1;
            
        // --- Detect walking ---
        az_list.push_back(az);
        if (az_list.size() > 15)
            az_list.erase(az_list.begin());
            
        if (empirical_standard_deviation(az_list) < 0.1f)
            moving = false;
        
        if (!moving) {
            if (az_list[14] > 0.3f)
                moving = true;
        }
        
        // --- Detect slopes ---
        if (!standby) {
            alt_list_new.push_back(altitude);
            if (alt_list_new.size() > 15)
                alt_list_new.erase(alt_list_new.begin());
            alt_list_prev.push_back(alt_list_new[0]);
            if (alt_list_prev.size() > 15)
                alt_list_prev.erase(alt_list_prev.begin());
            
            float slope = (mean(alt_list_new) - mean(alt_list_prev)) / (float)alt_list_new.size();
            if (slope > 0.02f) {
                if (!on_slope) {
                    if (!vector_contains(warnings, WARN_UPHILL))
                        warnings.push_back({WARN_UPHILL, 0, 0, 200});
                    on_slope = true;
                }
            }
            else if (slope < -0.02f) {
                if (!on_slope) {
                    if (!vector_contains(warnings, WARN_DOWNHILL))
                        warnings.push_back({WARN_DOWNHILL, 0, 0, 210});
                    on_slope = true;
                }
            }
            else if (on_slope)
                on_slope = false;
        
        
        // --- Cropping unnecessary parts of the frame when turning ---
            if (gy < -20.0f || gy > 20.0f) {
                cv::Mat frame_cr;
                int x_left = 0;
                int x_width = frame.size().width;
                if (gy < 0.0f) {
                    x_left += static_cast<int>(gy * 2) * -1;
                    x_width -= x_left + 1;
                }
                else
                    x_width -= static_cast<int>(gy * 2);
                frame_cr = frame(cv::Rect(x_left, 0, x_width, 720));
                frame = frame_cr;
            }
        
        // --- Image classification ---
            Imclass image_class = get_image_class(&frame);
            if (image_class != image_class_prev && image_class_counter == 0) {
                image_class_counter += 1;
                image_class_edge = image_class;
            }
            if (image_class_counter != 0)
                image_class_counter += 1;
            if (image_class_counter == 7) {
                if (image_class == image_class_edge) {
                    if (image_class == Imclass::Day) {
                        if (!vector_contains(warnings, TO_DAY))
                            warnings.push_back({TO_DAY, 0, 0, 150});
                        mobilenet = new Network(modelConfiguration, modelBinary);
                        mobilenet->initialize();
                    }
                    if (image_class == Imclass::Night) {
                        if (!vector_contains(warnings, TO_NIGHT))
                            warnings.push_back({TO_NIGHT, 0, 0, 151});
                        mobilenet = nullptr;
                    }
                }
                image_class_counter = 0;
                image_class_edge = Imclass::None;
            }
            image_class_prev = image_class;
        
        // --- Blur detection ---
            lap = laplacian(&frame);
        }
        
        // --- Object detection ---
        if (mobilenet != nullptr && lap > 40.0f) {
            detections = mobilenet->detect(frame);
            
            for (int i = 0; i < detections.rows; i++) {
                float confidence = detections.at<float>(i, 2);
            
                if (confidence > 0.3f) {
                    int objectClass = static_cast<int>(detections.at<float>(i, 1));
                    if (objectClass == 0)
                        continue;
                    int xLeftBottom = static_cast<int>(detections.at<float>(i, 3) * frame.cols);
                    int yLeftBottom = static_cast<int>(detections.at<float>(i, 4) * frame.rows);
                    int xRightTop = static_cast<int>(detections.at<float>(i, 5) * frame.cols);
                    int yRightTop = static_cast<int>(detections.at<float>(i, 6) * frame.rows);
                    float d = (int)std::sqrt(std::pow(xRightTop - xLeftBottom, 2) + std::pow(yLeftBottom - yRightTop, 2));
                    cv::Point m((int)((xLeftBottom + xRightTop) / 2), (int)((yLeftBottom + yRightTop) / 2));
                    std::cout << d << std::endl;
                    if (m.x < 0.3f * frame.size().width || m.x > 0.7f * frame.size().width)
                        continue;
                    
                    switch (objectClass) {
                        case 1: { // Person
                            if (d > 500.0f && distance_mean <= 500 && !vector_contains(warnings, WARN_PERSON))
                                warnings.push_back({WARN_PERSON, 0, 0, 300});
                        } break;
                        case 2: { // Bicycle
                            if (d > 400.0f && !vector_contains(warnings, WARN_BICYCLE))
                                warnings.push_back({WARN_BICYCLE, 0, 0, 400});
                        } break;
                        case 3: { // Car
                            if (d > 600.0f && distance_mean <= 500 && moving && !vector_contains(warnings, WARN_CAR))
                                warnings.push_back({WARN_CAR, 0, 0, 500});
                        } break;
                        case 4: { // Motorcycle
                            if (d > 550.0f && moving && !vector_contains(warnings, WARN_MOTORCYCLE))
                                warnings.push_back({WARN_MOTORCYCLE, 0, 0, 350});
                        } break;
                        case 5: { // Bus
                            if (d > 700.0f && distance_mean <= 500 && moving && !vector_contains(warnings, WARN_BUS))
                                warnings.push_back({WARN_BUS, 0, 0, 325});
                        } break;
                        case 6: { // Bench
                            if (d > 350.0f && d < 600.0f && !vector_contains(warnings, SUGG_BENCH))
                                warnings.push_back({SUGG_BENCH, 0, 0, 90});
                        } break;
                        case 7: { // Chair
                            if (d > 300.0f && d < 500.0f && !vector_contains(warnings, SUGG_CHAIR))
                                warnings.push_back({SUGG_CHAIR, 0, 0, 80});
                        } break;
                        case 8: { // Bin
                            if (d > 300.0f && d < 600.0f && !vector_contains(warnings, SUGG_BIN))
                                warnings.push_back({SUGG_BIN, 0, 0, 100});
                        } break;
                        case 9: { // Red traffic light
                            if (d > 100.0f && d < 300.0f && !moving) {
                                if (trafficlight_switch == -1 && !vector_contains(warnings, WARN_LIGHTRED))
                                    warnings.push_back({WARN_LIGHTRED, 0, 0, 600});
                                trafficlight_switch = 0;
                                trafficlight_counter = 1;
                            }
                        } break;
                        case 10: { // Green traffic light
                            if (trafficlight_switch == 0 && d > 100.0f && d < 300.0f && !moving && !vector_contains(warnings, WARN_LIGHTGREEN)) {
                                warnings.push_back({WARN_LIGHTGREEN, 0, 0, 550});
                                trafficlight_switch = 1;
                            }
                        } break;
                    }
                }
            }
            
            if (trafficlight_counter != 0)
                trafficlight_counter += 1;
            if (trafficlight_counter == 30) {
                trafficlight_counter = 0;
                trafficlight_switch = -1;
            }
            
            //mobilenet->draw_detections(frame);
        }
        
        
        
        /*cv::imshow("GUIDE-Walk v2.0", frame);
        if (cv::waitKey(10) == 99)
            break;*/
        if (quit)
            break;
        
        process_warnings();
    }
    
    stop = true;
    distanceThread.join();
    player->play_sample(SHUTDOWN, 1);
    std::this_thread::sleep_for(std::chrono::seconds(7));
    
    // --- End all processes ---
    videoThread.join();
    cap.release();
    warnings.clear();
    az_list.clear();
    alt_list_new.clear();
    alt_list_prev.clear();
    
    delete mobilenet;       // Delete MobileNet
    mobilenet = nullptr;
    delete motion_sen;      // Delete motion sensor
    motion_sen = nullptr;
    delete lidar;           // Delete lidar sensor
    lidar = nullptr;
    delete player;          // Delete audio player
    player = nullptr;
    
    system("sudo /bin/sh -c shutdown -h now"); // Shut down Jetson Nano
    return 0;
}
