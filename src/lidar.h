/*
    Original code by: Garmin International (<https://github.com/garmin>)
    
    lidar.h: Class header for Lidar Lite v3
    Code was taken from: <https://github.com/garmin/LIDARLite_RaspberryPi_Library>
    
    ORIGINAL LICENCE:
    Copyright (c) 2019 Garmin Ltd. or its subsidiaries.
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
#pragma once
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

#define LIDARLite_v3_h

// LIDAR-Lite default I2C device address
#define LIDARLITE_ADDR_DEFAULT 0x62

// LIDAR-Lite internal register addresses
#define LLv3_ACQ_CMD       0x00
#define LLv3_STATUS        0x01
#define LLv3_SIG_CNT_VAL   0x02
#define LLv3_ACQ_CONFIG    0x04
#define LLv3_DISTANCE      0x0f
#define LLv3_REF_CNT_VAL   0x12
#define LLv3_UNIT_ID_HIGH  0x16
#define LLv3_UNIT_ID_LOW   0x17
#define LLv3_I2C_ID_HIGH   0x18
#define LLv3_I2C_ID_LOW    0x19
#define LLv3_I2C_SEC_ADR   0x1a
#define LLv3_THRESH_BYPASS 0x1c
#define LLv3_I2C_CONFIG    0x1e
#define LLv3_COMMAND       0x40
#define LLv3_CORR_DATA     0x52
#define LLv3_ACQ_SETTINGS  0x5d

class LidarLite_v3 {
    __u32 file_i2c;
    
public:
    LidarLite_v3() {
        BOOST_LOG_TRIVIAL(info) << "Constructing Lidar class...";
    }
    ~LidarLite_v3() {
        BOOST_LOG_TRIVIAL(info) << "Destructing Lidar class...";
    }

    bool i2c_init(void) {
        char *filename = (char*)"/dev/i2c-1";

        if ((this->file_i2c = open(filename, O_RDWR)) < 0) {
           BOOST_LOG_TRIVIAL(error) << "Failed to open the i2c bus; Aborting.";
            return false;
        }
        else {
            return true;
        }
    }
    
    bool i2c_connect(__u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT) {
        if (ioctl(this->file_i2c, I2C_SLAVE, lidarliteAddress) < 0) {
            BOOST_LOG_TRIVIAL(info) << "Failed to acquire bus access and/or talk to slave; Aborting.";
            return false;
        }
        else {
            return true;
        }
    }
    
    void configure(__u8 configuration = 0, __u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT) {
        __u8 sigCountMax;
        __u8 acqConfigReg;
        __u8 refCountMax;
        __u8 thresholdBypass;
    
        switch (configuration)
        {
            case 0: // Default mode, balanced performance
                sigCountMax     = 0x80;
                acqConfigReg    = 0x08;
                refCountMax     = 0x05;
                thresholdBypass = 0x00;
                break;
        
            case 1: // Short range, high speed
                sigCountMax     = 0x1d;
                acqConfigReg    = 0x08;
                refCountMax     = 0x03;
                thresholdBypass = 0x00;
                break;
    
            case 2: // Default range, higher speed short range
                sigCountMax     = 0x80;
                acqConfigReg    = 0x00;
                refCountMax     = 0x03;
                thresholdBypass = 0x00;
                break;

            case 3: // Maximum range
                sigCountMax     = 0xff;
                acqConfigReg    = 0x08;
                refCountMax     = 0x05;
                thresholdBypass = 0x00;
                break;

            case 4: // High sensitivity detection, high erroneous measurements
                sigCountMax     = 0x80;
                acqConfigReg    = 0x08;
                refCountMax     = 0x05;
                thresholdBypass = 0x80;
                break;

            case 5: // Low sensitivity detection, low erroneous measurements
                sigCountMax     = 0x80;
                acqConfigReg    = 0x08;
                refCountMax     = 0x05;
                thresholdBypass = 0xb0;
                break;

            case 6: // Short range, high speed, higher error
                sigCountMax     = 0x04;
                acqConfigReg    = 0x01;
                refCountMax     = 0x03;
                thresholdBypass = 0x00;
                break;

            default: // Default mode, balanced performance - same as configure(0)
                sigCountMax     = 0x80;
                acqConfigReg    = 0x08;
                refCountMax     = 0x05;
                thresholdBypass = 0x00;
                break;
        }

        i2cWrite(LLv3_SIG_CNT_VAL,   &sigCountMax    , 1, lidarliteAddress);
        i2cWrite(LLv3_ACQ_CONFIG,    &acqConfigReg   , 1, lidarliteAddress);
        i2cWrite(LLv3_REF_CNT_VAL,   &refCountMax    , 1, lidarliteAddress);
        i2cWrite(LLv3_THRESH_BYPASS, &thresholdBypass, 1, lidarliteAddress);
    }
    
    void setI2Caddr(__u8 newAddress, __u8 disableDefault, __u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT) {
        __u8 dataBytes[2];

        i2cRead ((LLv3_UNIT_ID_HIGH | 0x80), dataBytes, 2, lidarliteAddress);
        i2cWrite(LLv3_I2C_ID_HIGH,           dataBytes, 2, lidarliteAddress);

        dataBytes[0] = newAddress;
        i2cWrite(LLv3_I2C_SEC_ADR,           dataBytes, 1, lidarliteAddress);
    
        dataBytes[0] = 0;
        i2cWrite(LLv3_I2C_CONFIG,            dataBytes, 1, lidarliteAddress);
    
        if (disableDefault) {
            dataBytes[0] = (1 << 3);
            i2cWrite(LLv3_I2C_CONFIG, dataBytes, 1, newAddress);
        }
    }
    
    __u16 readDistance(__u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT) {
         __u8  distBytes[2] = {0};

        i2cRead((LLv3_DISTANCE | 0x80), distBytes, 2, lidarliteAddress);
    
        return ((distBytes[0] << 8) | distBytes[1]);
    }
    
    void waitForBusy(__u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT) {
        __u8  busyFlag;

        do {
            busyFlag = getBusyFlag(lidarliteAddress);
        } while (busyFlag);
    }
    
    __u8 getBusyFlag(__u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT) {
        __u8  statusByte = 0;
        __u8  busyFlag;
    
        i2cRead(LLv3_STATUS, &statusByte, 1, lidarliteAddress);

        busyFlag = statusByte & 0x01;

        return busyFlag;
    }
    
    void takeRange(__u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT) {
        __u8 commandByte = 0x04;

        i2cWrite(LLv3_ACQ_CMD, &commandByte, 1, lidarliteAddress);
    }
    
    __s32 i2cWrite(__u8 regAddr, __u8 * dataBytes, __u8 numBytes, __u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT) {
        __u8 buffer[2];
        __u8 i;
        __s32 result;
    
        if (!i2c_connect(lidarliteAddress)) {
            return -1;
        }

        for (i = 0; i < numBytes; i++) {
            buffer[0] = regAddr + i;
            buffer[1] = dataBytes[i];
            result   |= write(this->file_i2c, buffer, 2);
        }

        return result;
    }
    
    __s32 i2cRead(__u8 regAddr, __u8 * dataBytes, __u8 numBytes, __u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT) {
        __u8 buffer;

        i2c_connect(lidarliteAddress);

        buffer = regAddr;

        write(file_i2c, &buffer, 1);
        return read(this->file_i2c, dataBytes, numBytes);
    }
    
    void correlationRecordRead(__s16 * correlationArray, __u16 numberOfReadings = 256, __u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT) {
        __u16  i = 0;
        __u8   dataBytes[2];
        __s16  correlationValue;
        __u8 * correlationValuePtr = (__u8 *) &correlationValue;

        dataBytes[0] = 0xc0;
        i2cWrite(LLv3_ACQ_SETTINGS, dataBytes, 1, lidarliteAddress);
    
        dataBytes[0] = 0x07;
        i2cWrite(LLv3_COMMAND, dataBytes, 1, lidarliteAddress);

        for (i = 0; i < numberOfReadings; i++) {
            i2cRead((LLv3_CORR_DATA | 0x80), dataBytes, 2, lidarliteAddress);

            correlationValuePtr[0] = dataBytes[0];

            if (dataBytes[1])
                correlationValuePtr[1] = 0xff;
            else
                correlationValuePtr[1] = 0x00;

            correlationArray[i] = correlationValue;
        }

        dataBytes[0] = 0;
        i2cWrite(LLv3_COMMAND, dataBytes, 1, lidarliteAddress);
    }
};