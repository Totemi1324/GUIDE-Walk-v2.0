/*
	Original code by: Simon D. Levy (<https://github.com/simondlevy>)
	
	I2C_core.h: Class header for USFS (Ultimate Sensor Fusion Solution)
	Code was taken from: <https://github.com/simondlevy/USFS>

	ORIGINAL LICENCE:
	Copyright (C) 2018 Simon D. Levy
	This file is part of USFS.
	
	USFS is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	USFS is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with USFS.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <stdint.h>
#include <time.h>
#include "I2C_core.h"
#if defined(ARDUINO)
#include <Arduino.h>
#elif defined(__arm__) 
#if defined(STM32F303)  || defined(STM32F405xx)
extern "C" {
	void delay(uint32_t msec);
}
#else
#include <wiringPi.h>
#endif

#else
void delay(unsigned int mseconds) {
    clock_t goal = mseconds + clock();
    while (goal > clock());
}
#endif

class Usfs {

    friend class USFS;

    protected:

        // SENtral register map
        // see http://www.emdeveloper.com/downloads/7180/EMSentral_USFS_Register_Map_v1_3.pdf
        static const uint8_t QX                 = 0x00;  // this is a 32-bit normalized floating point number read from registers = 0x00-03
        static const uint8_t QY                 = 0x04;  // this is a 32-bit normalized floating point number read from registers = 0x04-07
        static const uint8_t QZ                 = 0x08;  // this is a 32-bit normalized floating point number read from registers = 0x08-0B
        static const uint8_t QW                 = 0x0C;  // this is a 32-bit normalized floating point number read from registers = 0x0C-0F
        static const uint8_t QTIME              = 0x10;  // this is a 16-bit unsigned integer read from registers = 0x10-11
        static const uint8_t MX                 = 0x12;  // int16_t from registers = 0x12-13
        static const uint8_t MY                 = 0x14;  // int16_t from registers = 0x14-15
        static const uint8_t MZ                 = 0x16;  // int16_t from registers = 0x16-17
        static const uint8_t MTIME              = 0x18;  // uint16_t from registers = 0x18-19
        static const uint8_t AX                 = 0x1A;  // int16_t from registers = 0x1A-1B
        static const uint8_t AY                 = 0x1C;  // int16_t from registers = 0x1C-1D
        static const uint8_t AZ                 = 0x1E;  // int16_t from registers = 0x1E-1F
        static const uint8_t ATIME              = 0x20;  // uint16_t from registers = 0x20-21
        static const uint8_t GX                 = 0x22;  // int16_t from registers = 0x22-23
        static const uint8_t GY                 = 0x24;  // int16_t from registers = 0x24-25
        static const uint8_t GZ                 = 0x26;  // int16_t from registers = 0x26-27
        static const uint8_t GTIME              = 0x28;  // uint16_t from registers = 0x28-29
        static const uint8_t Baro               = 0x2A;  // start of two-byte MS5637 pressure data, 16-bit signed interger
        static const uint8_t BaroTIME           = 0x2C;  // start of two-byte MS5637 pressure timestamp, 16-bit unsigned
        static const uint8_t Temp               = 0x2E;  // start of two-byte MS5637 temperature data, 16-bit signed interger
        static const uint8_t TempTIME           = 0x30;  // start of two-byte MS5637 temperature timestamp, 16-bit unsigned
        static const uint8_t QRateDivisor       = 0x32;  // uint8_t 
        static const uint8_t EnableEvents       = 0x33;
        static const uint8_t HostControl        = 0x34;
        static const uint8_t EventStatus        = 0x35;
        static const uint8_t SensorStatus       = 0x36;
        static const uint8_t SentralStatus      = 0x37;
        static const uint8_t AlgorithmStatus    = 0x38;
        static const uint8_t FeatureFlags       = 0x39;
        static const uint8_t ParamAcknowledge   = 0x3A;
        static const uint8_t SavedParamByte0    = 0x3B;
        static const uint8_t SavedParamByte1    = 0x3C;
        static const uint8_t SavedParamByte2    = 0x3D;
        static const uint8_t SavedParamByte3    = 0x3E;
        static const uint8_t ActualMagRate      = 0x45;
        static const uint8_t ActualAccelRate    = 0x46;
        static const uint8_t ActualGyroRate     = 0x47;
        static const uint8_t ActualBaroRate     = 0x48;
        static const uint8_t ActualTempRate     = 0x49;
        static const uint8_t ErrorRegister      = 0x50;
        static const uint8_t AlgorithmControl   = 0x54;
        static const uint8_t MagRate            = 0x55;
        static const uint8_t AccelRate          = 0x56;
        static const uint8_t GyroRate           = 0x57;
        static const uint8_t BaroRate           = 0x58;
        static const uint8_t TempRate           = 0x59;
        static const uint8_t LoadParamByte0     = 0x60;
        static const uint8_t LoadParamByte1     = 0x61;
        static const uint8_t LoadParamByte2     = 0x62;
        static const uint8_t LoadParamByte3     = 0x63;
        static const uint8_t ParamRequest       = 0x64;
        static const uint8_t ROMVersion1        = 0x70;
        static const uint8_t ROMVersion2        = 0x71;
        static const uint8_t RAMVersion1        = 0x72;
        static const uint8_t RAMVersion2        = 0x73;
        static const uint8_t ProductID          = 0x90;
        static const uint8_t RevisionID         = 0x91;
        static const uint8_t RunStatus          = 0x92;
        static const uint8_t UploadAddress      = 0x94; // uint16_t registers = 0x94 (MSB)-5(LSB)
        static const uint8_t UploadData         = 0x96;  
        static const uint8_t CRCHost            = 0x97; // uint32_t from registers = 0x97-9A
        static const uint8_t ResetRequest       = 0x9B;   
        static const uint8_t PassThruStatus     = 0x9E;   
        static const uint8_t PassThruControl    = 0xA0;
        static const uint8_t ACC_LPF_BW         = 0x5B;  //Register GP36
        static const uint8_t GYRO_LPF_BW        = 0x5C;  //Register GP37
        static const uint8_t BARO_LPF_BW        = 0x5D;  //Register GP38
        static const uint8_t GP36               = 0x5B;
        static const uint8_t GP37               = 0x5C;
        static const uint8_t GP38               = 0x5D;
        static const uint8_t GP39               = 0x5E;
        static const uint8_t GP40               = 0x5F;
        static const uint8_t GP50               = 0x69;
        static const uint8_t GP51               = 0x6A;
        static const uint8_t GP52               = 0x6B;
        static const uint8_t GP53               = 0x6C;
        static const uint8_t GP54               = 0x6D;
        static const uint8_t GP55               = 0x6E;
        static const uint8_t GP56               = 0x6F;

        static const uint8_t ADDRESS           = 0x28;   // Address of the USFS SENtral sensor hub


        bool hasFeature(uint8_t features) {
			return features & readRegister(FeatureFlags);
		}

        static const uint8_t TEMP_OUT_H       = 0x41;
        static const uint8_t TEMP_OUT_L       = 0x42;

        // Cross-platform support
        uint8_t _i2c;

        uint8_t errorStatus;


        void readThreeAxis(uint8_t xreg, int16_t & x, int16_t & y, int16_t & z) {
			uint8_t rawData[6];  // x/y/z register data stored here
			readRegisters(xreg, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
			x = (int16_t) (((int16_t)rawData[1] << 8) | rawData[0]);   // Turn the MSB and LSB into a signed 16-bit value
			y = (int16_t) (((int16_t)rawData[3] << 8) | rawData[2]);  
			z = (int16_t) (((int16_t)rawData[5] << 8) | rawData[4]); 
		}

        uint8_t readRegister(uint8_t subAddress) {
			uint8_t data;
			readRegisters(subAddress, 1, &data);
			return data;                       
		}
        void readRegisters(uint8_t subAddress, uint8_t count, uint8_t * dest) {
			cpi2c_readRegisters(_i2c, subAddress, count, dest);
		}
        void writeRegister(uint8_t subAddress, uint8_t data) {
			 cpi2c_writeRegister(_i2c, subAddress, data);
		}

    public:

        bool begin(uint8_t bus=1) {
			_i2c = cpi2c_open(ADDRESS, bus);
			
			errorStatus = 0;
		
			// Check SENtral status, make sure EEPROM upload of firmware was accomplished
			for (int attempts=0; attempts<10; ++attempts) {
				if (readRegister(SentralStatus) & 0x01) {
					if(readRegister(SentralStatus) & 0x01) { }
					if(readRegister(SentralStatus) & 0x02) { }
					if(readRegister(SentralStatus) & 0x04) {
						errorStatus = 0xB0;
						return false;
					}
					if(readRegister(SentralStatus) & 0x08) { }
					if(readRegister(SentralStatus) & 0x10) {
						errorStatus = 0xB0;
						return false;
					}
					break;
				}
				writeRegister(ResetRequest, 0x01);
				delay(500);  
			}
		
			if (readRegister(SentralStatus) & 0x04) {
				errorStatus = 0xB0;
				return false;
			}

			return true;
		}

        const char * getErrorString(void) {
			if (errorStatus & 0x01) return "Magnetometer error";
			if (errorStatus & 0x02) return "Accelerometer error";
			if (errorStatus & 0x04) return "Gyro error";
			if (errorStatus & 0x10) return "Magnetometer ID not recognized";
			if (errorStatus & 0x20) return "Accelerometer ID not recognized";
			if (errorStatus & 0x30) return "Math error";
			if (errorStatus & 0x40) return "Gyro ID not recognized";
			if (errorStatus & 0x80) return "Invalid sample rate";
		
			// Ad-hoc
			if (errorStatus & 0x90) return "Failed to put SENtral in pass-through mode";
			if (errorStatus & 0xA0) return "Unable to read from SENtral EEPROM";
			if (errorStatus & 0xB0) return "Unable to upload config to SENtral EEPROM";
		
			return "Unknown error";
		}

        uint8_t  getProductId(void) {
			return readRegister(ProductID);
		}
        uint8_t  getRevisionId(void) {
			return readRegister(RevisionID);
		}
        uint16_t getRamVersion(void) {
			uint16_t ram1 = readRegister(RAMVersion1);
			uint16_t ram2 = readRegister(RAMVersion2);

			return ram1 << 8 | ram2;
		}
        uint16_t getRomVersion(void) {
			uint16_t rom1 = readRegister(ROMVersion1);
			uint16_t rom2 = readRegister(ROMVersion2);

			return rom1 << 8 | rom2;
		}

        uint8_t  getSentralStatus(void) {
			return readRegister(SentralStatus);
		}

        void requestReset(void) {
			writeRegister(ResetRequest, 0x01);
		}
        void setPassThroughMode(void) {
			// First put SENtral in standby mode
			writeRegister(AlgorithmControl, 0x01);
			delay(5);

			// Place SENtral in pass-through mode
			writeRegister(PassThruControl, 0x01);
			while (true) {
				if (readRegister(PassThruStatus) & 0x01) break;
				delay(5);
			}
		}
        void setMasterMode(void) {
			// Cancel pass-through mode
			writeRegister(PassThruControl, 0x00);
			while (true) {
				if (!(readRegister(PassThruStatus) & 0x01)) break;
				delay(5);
			}
		
			// Re-start algorithm
			writeRegister(AlgorithmControl, 0x00);
			while (true) {
				if (!(readRegister(AlgorithmStatus) & 0x01)) break;
				delay(5);
			}
		}
        void setRunEnable(void) {
			writeRegister(HostControl, 0x01);
		}
        void setRunDisable(void) {
			writeRegister(HostControl, 0x00);
		}

        void setAccelLpfBandwidth(uint8_t bw) {
			writeRegister(ACC_LPF_BW, bw);
		}
        void setGyroLpfBandwidth(uint8_t bw) {
			writeRegister(GYRO_LPF_BW, bw);
		}

        void setQRateDivisor(uint8_t divisor) {
			writeRegister(QRateDivisor, divisor);
		}
        void setMagRate(uint8_t rate) {
			writeRegister(MagRate, rate);
		}
        void setAccelRate(uint8_t rate) {
			writeRegister(AccelRate, rate);
		}
        void setGyroRate(uint8_t rate) {
			writeRegister(GyroRate, rate);
		}
        void setBaroRate(uint8_t rate) {
			 writeRegister(BaroRate, rate);
		}

        uint8_t  getActualMagRate() {
			return readRegister(ActualMagRate);
		}
        uint16_t getActualAccelRate() {
			return readRegister(ActualAccelRate);
		}
        uint16_t getActualGyroRate() {
			return readRegister(ActualGyroRate);
		}
        uint8_t  getActualBaroRate() {
			return readRegister(ActualBaroRate);
		}
        uint8_t  getActualTempRate() {
			return readRegister(ActualTempRate);
		}

        void algorithmControlRequestParameterTransfer(void) {
			writeRegister(AlgorithmControl, 0x80);
		}
        void algorithmControlReset(void) {
			writeRegister(AlgorithmControl, 0x00);
		}

        void enableEvents(uint8_t mask) {
			writeRegister(EnableEvents, mask);
		}

        void requestParamRead(uint8_t param) {
			writeRegister(ParamRequest, param);
		}
        uint8_t getParamAcknowledge(void) {
			return readRegister(ParamAcknowledge);
		}
        uint8_t readSavedParamByte0(void) {
			return readRegister(SavedParamByte0);
		}
        uint8_t readSavedParamByte1(void) {
			return readRegister(SavedParamByte1);
		}
        uint8_t readSavedParamByte2(void) {
			return readRegister(SavedParamByte2);
		}
        uint8_t readSavedParamByte3(void) {
			return readRegister(SavedParamByte3);
		}

        uint8_t getRunStatus(void) {
			return readRegister(RunStatus);
		}
        uint8_t getAlgorithmStatus(void) {
			return readRegister(AlgorithmStatus);
		}
        uint8_t getPassThruStatus(void) {
			return readRegister(PassThruStatus);
		}
        uint8_t getEventStatus(void) {
			return readRegister(EventStatus);
		}
        uint8_t getSensorStatus(void) {
			return readRegister(SensorStatus);
		}
        uint8_t getErrorStatus(void) {
			return readRegister(ErrorRegister);
		}

        void setGyroFs(uint16_t gyro_fs) {
			uint8_t bytes[4], STAT;
			bytes[0] = gyro_fs & (0xFF);
			bytes[1] = (gyro_fs >> 8) & (0xFF);
			bytes[2] = 0x00;
			bytes[3] = 0x00;
			writeRegister(LoadParamByte0, bytes[0]); //Gyro LSB
			writeRegister(LoadParamByte1, bytes[1]); //Gyro MSB
			writeRegister(LoadParamByte2, bytes[2]); //Unused
			writeRegister(LoadParamByte3, bytes[3]); //Unused
			writeRegister(ParamRequest, 0xCB); //Parameter 75; 0xCB is 75 decimal with the MSB set high to indicate a paramter write processs
			writeRegister(AlgorithmControl, 0x80); //Request parameter transfer procedure
			STAT = readRegister(ParamAcknowledge); //Check the parameter acknowledge register and loop until the result matches parameter request byte
			while(!(STAT==0xCB)) {
				STAT = readRegister(ParamAcknowledge);
			}
			writeRegister(ParamRequest, 0x00); //Parameter request = 0 to end parameter transfer process
			writeRegister(AlgorithmControl, 0x00); // Re-start algorithm
		}
        void setMagAccFs(uint16_t mag_fs, uint16_t acc_fs) {
			uint8_t bytes[4], STAT;
			bytes[0] = mag_fs & (0xFF);
			bytes[1] = (mag_fs >> 8) & (0xFF);
			bytes[2] = acc_fs & (0xFF);
			bytes[3] = (acc_fs >> 8) & (0xFF);
			writeRegister(LoadParamByte0, bytes[0]); //Mag LSB
			writeRegister(LoadParamByte1, bytes[1]); //Mag MSB
			writeRegister(LoadParamByte2, bytes[2]); //Acc LSB
			writeRegister(LoadParamByte3, bytes[3]); //Acc MSB
			writeRegister(ParamRequest, 0xCA); //Parameter 74; 0xCA is 74 decimal with the MSB set high to indicate a paramter write processs
			writeRegister(AlgorithmControl, 0x80); //Request parameter transfer procedure
			STAT = readRegister(ParamAcknowledge); //Check the parameter acknowledge register and loop until the result matches parameter request byte
			while(!(STAT==0xCA)) {
				STAT = readRegister(ParamAcknowledge);
			}
			writeRegister(ParamRequest, 0x00); //Parameter request = 0 to end parameter transfer process
			writeRegister(AlgorithmControl, 0x00); // Re-start algorithm
		}

        void loadParamByte0(uint8_t value) {
			 writeRegister(LoadParamByte0, value);
		}
        void loadParamByte1(uint8_t value) {
			writeRegister(LoadParamByte1, value);
		}
        void loadParamByte2(uint8_t value) {
			writeRegister(LoadParamByte2, value);
		}
        void loadParamByte3(uint8_t value) {
			writeRegister(LoadParamByte3, value);
		}

        void writeGp36(uint8_t value) {
			writeRegister(GP36, value);
		}
        void writeGp37(uint8_t value) {
			writeRegister(GP37, value);
		}
        void writeGp38(uint8_t value) {
			writeRegister(GP38, value);
		}
        void writeGp39(uint8_t value) {
			writeRegister(GP39, value);
		}
        void writeGp40(uint8_t value) {
			writeRegister(GP40, value);
		}
        void writeGp50(uint8_t value) {
			writeRegister(GP50, value);
		}
        void writeGp51(uint8_t value) {
			writeRegister(GP51, value);
		}
        void writeGp52(uint8_t value) {
			writeRegister(GP52, value);
		}
        void writeGp53(uint8_t value) {
			writeRegister(GP53, value);
		}
        void writeGp54(uint8_t value) {
			writeRegister(GP54, value);
		}
        void writeGp55(uint8_t value) {
			writeRegister(GP55, value);
		}
        void writeGp56(uint8_t value) {
			writeRegister(GP56, value);
		}

        void readAccelerometer(int16_t & ax, int16_t & ay, int16_t & az) {
			readThreeAxis(AX, ax, ay, az);
		}
        void readMagnetometer(int16_t & mx, int16_t & my, int16_t & mz) {
			readThreeAxis(MX, mx, my, mz);
		}
        void readGyrometer(int16_t & gx, int16_t & gy, int16_t & gz) {
			readThreeAxis(GX, gx, gy, gz);
		}
        void readQuaternion(float & qw, float & qx, float & qy, float & qz) {
			uint8_t rawData[16];  // x/y/z/w quaternion register data stored here (note unusual order!)

			readRegisters(QX, 16, &rawData[0]);       

			qx = uint32_reg_to_float (&rawData[0]);
			qy = uint32_reg_to_float (&rawData[4]);
			qz = uint32_reg_to_float (&rawData[8]);
			qw = uint32_reg_to_float (&rawData[12]);
		}
        void readBarometer(float & pressure, float & temperature) {
			uint8_t rawData[2];  // x/y/z gyro register data stored here

			readRegisters(Baro, 2, &rawData[0]);  // Read the two raw data registers sequentially into data array

			int16_t rawPressure =  (int16_t) (((int16_t)rawData[1] << 8) | rawData[0]);   // Turn the MSB and LSB into a signed 16-bit value
			pressure = (float)rawPressure *.01f + 1013.25f; // pressure in millibars

			// get BMP280 temperature
			readRegisters(Temp, 2, &rawData[0]);  // Read the two raw data registers sequentially into data array
			int16_t rawTemperature =  (int16_t) (((int16_t)rawData[1] << 8) | rawData[0]);   // Turn the MSB and LSB into a signed 16-bit value
		
			temperature = (float) rawTemperature*0.01;  // temperature in degrees C
		}

        void setIntegerParam (uint8_t param, uint32_t param_val) {
			uint8_t bytes[4], STAT;
			bytes[0] = param_val & (0xFF);
			bytes[1] = (param_val >> 8) & (0xFF);
			bytes[2] = (param_val >> 16) & (0xFF);
			bytes[3] = (param_val >> 24) & (0xFF);
			param = param | 0x80; //Parameter is the decimal value with the MSB set high to indicate a paramter write processs
			writeRegister(LoadParamByte0, bytes[0]); //Param LSB
			writeRegister(LoadParamByte1, bytes[1]);
			writeRegister(LoadParamByte2, bytes[2]);
			writeRegister(LoadParamByte3, bytes[3]); //Param MSB
			writeRegister(ParamRequest, param);
			writeRegister(AlgorithmControl, 0x80); //Request parameter transfer procedure
			STAT = readRegister(ParamAcknowledge); //Check the parameter acknowledge register and loop until the result matches parameter request byte
			while(!(STAT==param)) {
				STAT = readRegister(ParamAcknowledge);
			}
			writeRegister(ParamRequest, 0x00); //Parameter request = 0 to end parameter transfer process
			writeRegister(AlgorithmControl, 0x00); // Re-start algorithm
		}

        void getFullScaleRanges(uint8_t& accFs, uint16_t& gyroFs, uint16_t& magFs) {
			uint8_t param[4];

			// Read sensor new FS values from parameter space
			writeRegister(ParamRequest, 0x4A); // Request to read  parameter 74
			writeRegister(AlgorithmControl, 0x80); // Request parameter transfer process
			uint8_t param_xfer = readRegister(ParamAcknowledge);
			while(!(param_xfer==0x4A)) {
				param_xfer = readRegister(ParamAcknowledge);
			}
			param[0] = readRegister(SavedParamByte0);
			param[1] = readRegister(SavedParamByte1);
			param[2] = readRegister(SavedParamByte2);
			param[3] = readRegister(SavedParamByte3);
			magFs = ((uint16_t)(param[1]<<8) | param[0]);
			accFs = ((uint8_t)(param[3]<<8) | param[2]);
			writeRegister(ParamRequest, 0x4B); // Request to read  parameter 75
			param_xfer = readRegister(ParamAcknowledge);
			while(!(param_xfer==0x4B)) {
				param_xfer = readRegister(ParamAcknowledge);
			}
			param[0] = readRegister(SavedParamByte0);
			param[1] = readRegister(SavedParamByte1);
			param[2] = readRegister(SavedParamByte2);
			param[3] = readRegister(SavedParamByte3);
			gyroFs = ((uint16_t)(param[1]<<8) | param[0]);
			writeRegister(ParamRequest, 0x00); //End parameter transfer
			writeRegister(AlgorithmControl, 0x00); // re-enable algorithm
		}

        static float uint32_reg_to_float (uint8_t *buf) {
			union {
				uint32_t ui32;
				float f;
			} u;
		
			u.ui32 = (((uint32_t)buf[0]) +
					(((uint32_t)buf[1]) <<  8) +
					(((uint32_t)buf[2]) << 16) +
					(((uint32_t)buf[3]) << 24));
			return u.f;
		}

}; // class USFS