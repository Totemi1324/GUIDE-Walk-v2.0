#pragma once

#include "usfs.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

class USFS {
    private:

        Usfs _usfs;

        uint8_t _eventStatus;

        uint8_t _magRate;      // Hz
        uint16_t _accelRate;    // Hz
        uint16_t _gyroRate;     // Hz
        uint8_t _baroRate;     // Hz
        uint8_t _qRateDivisor; // w.r.t. gyro rate

        void readThreeAxis(uint8_t regx, float& x, float& y, float& z, float scale) {
            int16_t xx=0, yy=0, zz=0;

            _usfs.readThreeAxis(regx, xx, yy, zz);

            x = xx * scale;
            y = yy * scale;
            z = zz * scale;
        }

    public:

        USFS(uint8_t  magRate, uint16_t accelRate, uint16_t gyroRate, uint8_t  baroRate, uint8_t qRateDivisor) {
            BOOST_LOG_TRIVIAL(info) << "Constructing USFS class...";
            this->_magRate = magRate;
            this->_accelRate = accelRate;
            this->_gyroRate = gyroRate; 
            this->_baroRate = baroRate;
            this->_qRateDivisor = qRateDivisor;
        }
        ~USFS() {
            BOOST_LOG_TRIVIAL(info) << "Destructing USFS class...";
        }

        const char * getErrorString(void) {
            return _usfs.getErrorString();
        }

        bool begin(uint8_t bus=1) {
            BOOST_LOG_TRIVIAL(info) << "Initializing motion sensor...";
            if (!_usfs.begin(bus)) return false;

            // Enter USFS initialized state
            _usfs.setRunDisable();
            _usfs.setMasterMode();
            _usfs.setRunEnable();
            _usfs.setRunDisable();

            _usfs.setAccelLpfBandwidth(0x03); // 41Hz
            _usfs.setGyroLpfBandwidth(0x03);  // 41Hz

            _usfs.setQRateDivisor(_qRateDivisor-1);
            _usfs.setMagRate(_magRate);
            _usfs.setAccelRate(_accelRate/10);
            _usfs.setGyroRate(_gyroRate/10);
            _usfs.setBaroRate(0x80 | _baroRate);

            _usfs.algorithmControlReset();

            _usfs.enableEvents(0x07);

            // Enable USFS run mode
            _usfs.setRunEnable();
            delay(100);

            _usfs.setIntegerParam (0x49, 0x00);

            BOOST_LOG_TRIVIAL(info) << "Done initializing motion sensor!";
            return _usfs.getSensorStatus() ? false : true;
        }

        void checkEventStatus(void) {
            _eventStatus = _usfs.getEventStatus();
        }

        bool gotError(void) {
            if (_eventStatus & 0x02) {
                return true;
            }

            return false;
        }

        bool gotQuaternion(void) {
            return _eventStatus & 0x04;
        }

        bool gotMagnetometer(void) {
            return _eventStatus & 0x08;
        }

        bool gotAccelerometer(void) {
            return _eventStatus & 0x10;
        }

        bool gotGyrometer(void) {
            return _eventStatus & 0x20;
        }

        bool gotBarometer(void) {
            return _eventStatus & 0x40;
        }

        void readMagnetometer(float& mx, float& my, float& mz) {
            readThreeAxis(Usfs::MX, mx, my, mz, 0.305176);
        }

        void readAccelerometer(float& ax, float& ay, float& az) {
            readThreeAxis(Usfs::AX, ax, ay, az, 0.000488);
        }

        void readGyrometer(float& gx, float& gy, float& gz) {
            readThreeAxis(Usfs::GX, gx, gy, gz, 0.153);
        }

        void readQuaternion(float& qw, float& qx, float& qy, float& qz) {
            _usfs.readQuaternion(qw, qx, qy, qz);
        }

        void readBarometer(float& pressure, float& temperature) {
            _usfs.readBarometer(pressure, temperature);
        }

        void Magnetometer(float& mx, float& my, float& mz) {
            if (gotMagnetometer())
                readMagnetometer(mx, my, mz);
            else
                BOOST_LOG_TRIVIAL(info) << "No magnetometer value provided.";
        }
        void Accelerometer(float& ax, float& ay, float& az) {
            if (gotAccelerometer())
                readAccelerometer(ax, ay, az);
            else
                BOOST_LOG_TRIVIAL(info) << "No accelerometer value provided.";
        }
        void Gyroscope(float& gx, float& gy, float& gz) {
            if (gotGyrometer())
                readGyrometer(gx, gy, gz);
            else
                BOOST_LOG_TRIVIAL(info) << "No accelerometer value provided.";
        }
        void Quaternion(float& roll, float& pitch, float& yaw) {
            if (gotQuaternion()) {
                float qw, qx, qy, qz;

                readQuaternion(qw, qx, qy, qz);

                roll  = atan2(2.0f * (qw * qx + qy * qz), qw * qw - qx * qx - qy * qy + qz * qz);
                pitch = -asin(2.0f * (qx * qz - qw * qy));
                yaw   = atan2(2.0f * (qx * qy + qw * qz), qw * qw + qx * qx - qy * qy - qz * qz);   

                pitch *= 180.0f / M_PI;
                yaw *= 180.0f / M_PI; 
                yaw += 13.8f;
                if(yaw < 0) yaw += 360.0f;
                roll  *= 180.0f / M_PI;
            }
            else
                BOOST_LOG_TRIVIAL(info) << "No quaternion value provided.";
        }
        void Barometer(float& pressure, float& temperature, float& altitude) {
            if (gotBarometer()) {
                readBarometer(pressure, temperature); 
                altitude = (1.0f - powf(pressure / 1013.25f, 0.190295f)) * 44330.0f;
            }
            else
                BOOST_LOG_TRIVIAL(info) << "No barometer value provided.";
        }
};
