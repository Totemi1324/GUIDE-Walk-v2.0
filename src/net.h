#pragma once
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/shape_utils.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <fstream>
#include <stdio.h>
#include <ctime>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

std::string gstreamer_pipeline(int capture_width, int capture_height, int display_width, int display_height, int framer, int flip_method) {
    return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(capture_width) + ", height=(int)" +
           std::to_string(capture_height) + ", format=(string)NV12, framerate=(fraction)" + std::to_string(framer) +
           "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
           std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

double clock_to_millisecs(clock_t ticks) {
    // Convert ctime-ticks to milliseconds
    return (ticks / (double)CLOCKS_PER_SEC) * 1000.0;
}

int clamp(int n, int lo, int hi) {
    return std::max(lo, std::min(n, hi));
}

int random_range(int low, int high) {
    static bool first = true;
    if (first) {
        srand(time(NULL));
        first = false;
    }
    return low + rand() % ((high + 1) - low);
}

float mean(std::vector<float> &vec) {
    float mean_val = 0;
    for (std::size_t i = 0; i < vec.size(); i++) {
        mean_val += vec[i];
    }
    return mean_val / (float)vec.size();
}

float empirical_standard_deviation(std::vector<float> &vec) {
    float mean_val = 0;
    for (std::size_t i = 0; i < vec.size(); i++) {
        mean_val += vec[i];
    }
    mean_val = mean_val / 15.0f;
    float var_sum = 0;
    for (std::size_t i = 0; i < vec.size(); i++) {
        var_sum += std::pow(vec[i] - mean_val, 2);
    }
    return std::sqrt((1.0f / ((float)vec.size() - 1)) * var_sum);
}

bool vector_contains(std::vector<std::array<int, 4>> vec, int label) {
    for (unsigned int i = 0; i < vec.size(); i++) {
        if (vec[i][0] == label)
            return true;
    }

    return false;
}

class Network {
private:
    std::string modelConfig;
    std::string modelBin;
    cv::dnn::Net net;
    cv::Mat detectionMat;
    
    const std::size_t inWidth = 300;
    const std::size_t inHeight = 300;
    const float inScaleFactor = 0.007843f;
    const float meanVal = 127.5f;
    const float confidenceThreshold = 0.3f;
    
    char classNames[11][19] = {"background", "person", "car", "bus", "bicycle", "motorcycle", "bench", "chair", "bin", "traffiglight_red", "trafficlight_green"};
    
    cv::Scalar colors[11] = {cv::Scalar(0, 0, 0), cv::Scalar(0, 255, 128), cv::Scalar(0, 255, 255), cv::Scalar(0, 128, 255), cv::Scalar(128, 255, 0), cv::Scalar(255, 255, 0), cv::Scalar(255, 128, 0), cv::Scalar(255, 0, 127), cv::Scalar(255, 0, 255), cv::Scalar(0, 0, 204), cv::Scalar(0, 204, 0)};
public:
    Network(std::string modelConfig, std::string modelBin) {
        BOOST_LOG_TRIVIAL(info) << "Construsting network class...";
        this->modelConfig = modelConfig;
        this->modelBin = modelBin;
    }
    ~Network() {
        BOOST_LOG_TRIVIAL(info) << "Destructing network class...";
    }
    
    void initialize() {
        BOOST_LOG_TRIVIAL(info) << "Initializing network... ";
        this->net = cv::dnn::readNetFromCaffe(modelConfig, modelBin);
        this->net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA); // Activate GPU acceleration
        this->net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        BOOST_LOG_TRIVIAL(info) << "Done initializing network!";
    }
    cv::Mat detect(cv::Mat frame) {
        cv::Mat inputBlob = cv::dnn::blobFromImage(frame, inScaleFactor, cv::Size(inWidth, inHeight), cv::Scalar(meanVal, meanVal, meanVal), false); //Convert Mat to batch of images

        net.setInput(inputBlob, "data"); //Set the network input
        cv::Mat detection = net.forward("detection_out");
        cv::Mat detectMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
        
        this->detectionMat = detectMat;
        
        return detectMat;
    }
    void draw_detections(cv::Mat& frame) {
        for(int i = 0; i < this->detectionMat.rows; i++) {
            float confidence = this->detectionMat.at<float>(i, 2);
            
            if(confidence > confidenceThreshold) {
                std::size_t objectClass = (std::size_t)(this->detectionMat.at<float>(i, 1));
                if (objectClass == 0 || objectClass == 9 || objectClass == 10)
                    continue;

                int xLeftBottom = clamp(static_cast<int>(this->detectionMat.at<float>(i, 3) * frame.cols), 0, 1280);
                int yLeftBottom = clamp(static_cast<int>(this->detectionMat.at<float>(i, 4) * frame.rows), 0, 720);
                int xRightTop = clamp(static_cast<int>(this->detectionMat.at<float>(i, 5) * frame.cols), 0, 1280);
                int yRightTop = clamp(static_cast<int>(this->detectionMat.at<float>(i, 6) * frame.rows), 0, 720);

                std::ostringstream ss;
                ss << (confidence * 100);
                cv::String conf(ss.str());

                cv::Rect object(xLeftBottom, yLeftBottom, xRightTop - xLeftBottom, yRightTop - yLeftBottom);
                cv::Mat roi = frame(object);
                cv::Mat color(roi.size(), CV_8UC3, colors[objectClass]);
                double alpha = 0.3;
                cv::addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);

                cv::String label;
                label = cv::String(classNames[objectClass]) + ": " + conf + "%";
                int baseLine = 0;
                cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
                cv::rectangle(frame, cv::Rect(cv::Point(xLeftBottom, yLeftBottom - labelSize.height),
                                cv::Size(labelSize.width, labelSize.height + baseLine)),
                                cv::Scalar(255, 255, 255), cv::FILLED);
                cv::putText(frame, label, cv::Point(xLeftBottom, yLeftBottom),
                                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,0));
            }
        }
    }
};
