#pragma once
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <SDL2/SDL_mixer.h>
#include <SDL.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#define NUM_WAVEFORMS 41

#define STARTUP_SEQUENCE    0
#define STARTUP_WARNING	    1
#define SIGN_START          2
#define WARN_PERSON	    3
#define WARN_BICYCLE        4
#define WARN_CAR            5
#define WARN_MOTORCYCLE     6
#define WARN_BUS            7
#define WARN_LIGHTRED       8
#define WARN_LIGHTGREEN     9
#define WARN_UPHILL         10
#define WARN_DOWNHILL       11
#define SUGG_BENCH          12
#define SUGG_CHAIR          13
#define SUGG_BIN            14
#define TO_DAY              15
#define TO_NIGHT            16
#define FALLING             17
#define RAIN_YES            30
#define RAIN_NO             31
#define STANDBY_ON          32
#define STANDBY_OFF         33
#define ERROR_CAM           34
#define ERROR_USFS          35
#define ERROR_LIDAR         36
#define SHUTDOWN_CONF       37
#define SHUTDOWN            38
#define SHORT_BEEP          39
#define LONG_BEEP           40

class AudioPlayer {
private:
    const char _waveFileNames[NUM_WAVEFORMS][40] = {"audio/startup_seq.wav", "audio/startup_warning.wav", 
                                        "audio/start_signal.wav", "audio/warning_person.wav",
                                        "audio/warning_bicycle.wav", "audio/warning_car.wav",
                                        "audio/warning_motorcycle.wav", "audio/warning_bus.wav",
                                        "audio/warning_trafficlight_red.wav", "audio/warning_trafficlight_green.wav",
                                        "audio/warning_uphill.wav", "audio/warning_downhill.wav",
                                        "audio/suggestion_bench.wav", "audio/suggestion_chair.wav",
                                        "audio/suggestion_bin.wav","audio/to_day.wav", "audio/to_night.wav",
                                        "audio/falling.wav", "audio/clock_13.wav", "audio/clock_14.wav",
                                        "audio/clock_15.wav", "audio/clock_16.wav", "audio/clock_17.wav",
                                        "audio/clock_18.wav", "audio/temp_10.wav", "audio/temp_15.wav",
                                        "audio/temp_20.wav", "audio/temp_25.wav", "audio/temp_30.wav",
                                        "audio/temp_35.wav", "audio/rain_yes.wav", "audio/rain_no.wav",
                                        "audio/activate_standby.wav", "audio/deactivate_standby.wav",
                                        "audio/error_camera.wav", "audio/error_usfs.wav", "audio/error_lidar.wav",
                                        "audio/confirm_shutdown.wav", "audio/shutdown.wav",
                                        "audio/short_beep.wav", "audio/long_beep.wav"};
    Mix_Chunk* _sample[NUM_WAVEFORMS];
public:
    AudioPlayer() {
        BOOST_LOG_TRIVIAL(info) << "Constructing audio player class...";
        
        if (Mix_Init(MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG) < 0) {
            BOOST_LOG_TRIVIAL(error) << Mix_GetError();
            BOOST_LOG_TRIVIAL(error) << "Failed initializing Mixer; Aborting.";
            exit(-1);
        }
        
        memset(this->_sample, 0, sizeof(Mix_Chunk*) * 2);

        if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
            BOOST_LOG_TRIVIAL(error) << Mix_GetError();
            BOOST_LOG_TRIVIAL(error) << "Unable to open audio player; Aborting.";
            exit(-1);
        }
        
        Mix_AllocateChannels(2);
        
        for(int i = 0; i < NUM_WAVEFORMS; i++) {
            this->_sample[i] = Mix_LoadWAV(this->_waveFileNames[i]);
            if(this->_sample[i] == NULL) {
                BOOST_LOG_TRIVIAL(error) << "Unable to load wave file: " << this->_waveFileNames[i];
            }
        }
    }
    ~AudioPlayer() {
        BOOST_LOG_TRIVIAL(info) << "Destructing audio player class...";
        for(int i = 0; i < NUM_WAVEFORMS; i++) {
            Mix_FreeChunk(this->_sample[i]);
        }
        Mix_CloseAudio();
        Mix_Quit();
    }
    
    void play_sample(int _s, int _c) {
        Mix_PlayChannel(_c, this->_sample[_s], 0);
    }
    void set_volume(int _c, int _v) {
        Mix_Volume(_c, _v);
    }
    bool is_playing(int _c) {
        return Mix_Playing(_c);
    }
};
