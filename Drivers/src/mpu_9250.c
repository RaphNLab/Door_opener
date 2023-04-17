/*
 * mpu_9250.c
 *
 *  Created on: Mar 11, 2023
 *      Author: silvere
 */

#include "mpu_9250.h"
#include "spi.h"
#include "i2c.h"

uint8_t Ascale = AFS_2G;     // AFS_2G, AFS_4G, AFS_8G, AFS_16G
uint8_t Gscale = GFS_250DPS; // GFS_250DPS, GFS_500DPS, GFS_1000DPS, GFS_2000DPS


uint8_t MPU9250WhoAmI(void)
{
	uint8_t whoAmI = 0;
#ifdef USE_SPI
	whoAmI = spiReadByte(&spiDev, WHO_AM_I);
#else
	whoAmI = i2cReadByte(&i2cDev, (uint8_t)MPU9250_ADDR, (uint8_t)WHO_AM_I);
#endif
	return whoAmI;
}



void MPU9250Reset(void)
{
#ifdef USE_SPI
	spiWriteByte(&spiDev, PWR_MGMT_1, 0x80);
#else
	i2cWriteByte(&i2cDev, (uint8_t)MPU9250_ADDR, PWR_MGMT_1, 0x80);
#endif
}

void MPU9250Init(void)
{
#ifdef USE_SPI
	spiWriteByte(&spiDev, PWR_MGMT_1, 0x00);
	spiWriteByte(&spiDev, PWR_MGMT_1, 0x01);
	spiWriteByte(&spiDev, CONFIG, 0x03);

	uint8_t c = spiReadByte(&spiDev, ACCEL_CONFIG);

	c = c & ~0x18;  // Clear AFS bits [4:3]
	c = c | Ascale << 3; // Set full scale range for the accelerometer
	spiWriteByte(&spiDev, ACCEL_CONFIG, c);

#else

	i2cWriteByte(&i2cDev, MPU9250_ADDR, PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors
	i2cWriteByte(&i2cDev, MPU9250_ADDR, PWR_MGMT_1, 0x01); //Set clock source
	i2cWriteByte(&i2cDev, MPU9250_ADDR, CONFIG, 0x03);

	uint8_t c = i2cReadByte(&i2cDev, MPU9250_ADDR, ACCEL_CONFIG); // get current ACCEL_CONFIG register value
	// c = c & ~0xE0; // Clear self-test bits [7:5]
	c = c & ~0x18;  // Clear AFS bits [4:3]
	c = c | Ascale << 3; // Set full scale range for the accelerometer
	i2cWriteByte(&i2cDev, MPU9250_ADDR, ACCEL_CONFIG, c); // Write new ACCEL_CONFIG register value
#endif
}


void MPU9250Calibrate(float * dest1, float * dest2)
{
	uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
	uint16_t i, packetCount, fifoCount;
	int32_t gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0};

	// reset device, reset all registers, clear gyro and accelerometer bias registers
	i2cWriteByte(&i2cDev, MPU9250_ADDR, PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device

	// get stable time source
	// Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
	i2cWriteByte(&i2cDev, MPU9250_ADDR, PWR_MGMT_1, 0x01);
	i2cWriteByte(&i2cDev, MPU9250_ADDR, PWR_MGMT_2, 0x00);


	// Configure device for bias calculation
	i2cWriteByte(&i2cDev, MPU9250_ADDR, INT_ENABLE, 0x00);   // Disable all interrupts
	i2cWriteByte(&i2cDev, MPU9250_ADDR, FIFO_EN, 0x00);      // Disable FIFO
	i2cWriteByte(&i2cDev, MPU9250_ADDR, PWR_MGMT_1, 0x00);   // Turn on internal clock source
	i2cWriteByte(&i2cDev, MPU9250_ADDR, I2C_MST_CTRL, 0x00); // Disable I2C master
	i2cWriteByte(&i2cDev, MPU9250_ADDR, USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
	i2cWriteByte(&i2cDev, MPU9250_ADDR, USER_CTRL, 0x0C);    // Reset FIFO and DMP


	// Configure MPU9250 gyro and accelerometer for bias calculation
	i2cWriteByte(&i2cDev, MPU9250_ADDR, CONFIG, 0x01);      // Set low-pass filter to 188 Hz
	i2cWriteByte(&i2cDev, MPU9250_ADDR, SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
	i2cWriteByte(&i2cDev, MPU9250_ADDR, GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
	i2cWriteByte(&i2cDev, MPU9250_ADDR, ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity

	uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
	uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

	// Configure FIFO to capture accelerometer and gyro data for bias calculation
	i2cWriteByte(&i2cDev, MPU9250_ADDR, USER_CTRL, 0x40);   // Enable FIFO
	i2cWriteByte(&i2cDev, MPU9250_ADDR, FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250)

	// At end of sample accumulation, turn off FIFO sensor read
	i2cWriteByte(&i2cDev, MPU9250_ADDR, FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
	i2cReadBytes(&i2cDev, MPU9250_ADDR, FIFO_COUNTH, &data[0], 2); // read FIFO sample count
	fifoCount = ((uint16_t)data[0] << 8) | data[1];
	packetCount = fifoCount/12;// How many sets of full gyro and accelerometer data for averaging

	for (i = 0; i < packetCount; i++)
	{
		int16_t accelTemp[3] = {0, 0, 0}, gyroTemp[3] = {0, 0, 0};
		i2cReadBytes(&i2cDev, MPU9250_ADDR, FIFO_R_W, &data[0], 12); // read data for averaging
		accelTemp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
		accelTemp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
		accelTemp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;
		gyroTemp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
		gyroTemp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
		gyroTemp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;

		accelBias[0] += (int32_t) accelTemp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
		accelBias[1] += (int32_t) accelTemp[1];
		accelBias[2] += (int32_t) accelTemp[2];
		gyroBias[0]  += (int32_t) gyroTemp[0];
		gyroBias[1]  += (int32_t) gyroTemp[1];
		gyroBias[2]  += (int32_t) gyroTemp[2];

	}
	accelBias[0] /= (int32_t) packetCount; // Normalize sums to get average count biases
	accelBias[1] /= (int32_t) packetCount;
	accelBias[2] /= (int32_t) packetCount;
	gyroBias[0]  /= (int32_t) packetCount;
	gyroBias[1]  /= (int32_t) packetCount;
	gyroBias[2]  /= (int32_t) packetCount;

	if(accelBias[2] > 0L)
	{
	  accelBias[2] -= (int32_t) accelsensitivity; // Remove gravity from the z-axis accelerometer bias calculation
	}
	else
	{
	  accelBias[2] += (int32_t) accelsensitivity;
	}


	// Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
	data[0] = (-gyroBias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
	data[1] = (-gyroBias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
	data[2] = (-gyroBias[1]/4  >> 8) & 0xFF;
	data[3] = (-gyroBias[1]/4)       & 0xFF;
	data[4] = (-gyroBias[2]/4  >> 8) & 0xFF;
	data[5] = (-gyroBias[2]/4)       & 0xFF;

	dest1[0] = (float) gyroBias[0]/(float) gyrosensitivity; // construct gyro bias in deg/s for later manual subtraction
	dest1[1] = (float) gyroBias[1]/(float) gyrosensitivity;
	dest1[2] = (float) gyroBias[2]/(float) gyrosensitivity;


	int32_t accelBiasReg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
	i2cReadBytes(&i2cDev, MPU9250_ADDR, XA_OFFSET_H, &data[0], 2); // Read factory accelerometer trim values
	accelBiasReg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
	i2cReadBytes(&i2cDev, MPU9250_ADDR, YA_OFFSET_H, &data[0], 2);
	accelBiasReg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
	i2cReadBytes(&i2cDev, MPU9250_ADDR, ZA_OFFSET_H, &data[0], 2);
	accelBiasReg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];

	uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
	uint8_t maskBit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis

	for(i = 0; i < 3; i++)
	{
	    if(accelBiasReg[i] & mask)
	    	maskBit[i] = 0x01; // If temperature compensation bit is set, record that fact in maskBit
	}

	// Construct total accelerometer bias, including calculated average accelerometer bias from above
	accelBiasReg[0] -= (accelBias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
	accelBiasReg[1] -= (accelBias[1]/8);
	accelBiasReg[2] -= (accelBias[2]/8);

	data[0] = (accelBiasReg[0] >> 8) & 0xFF;
	data[1] = (accelBiasReg[0])      & 0xFF;
	data[1] = data[1] | maskBit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
	data[2] = (accelBiasReg[1] >> 8) & 0xFF;
	data[3] = (accelBiasReg[1])      & 0xFF;
	data[3] = data[3] | maskBit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
	data[4] = (accelBiasReg[2] >> 8) & 0xFF;
	data[5] = (accelBiasReg[2])      & 0xFF;
	data[5] = data[5] | maskBit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

	// Output scaled accelerometer biases for manual subtraction in the main program
	dest2[0] = (float)accelBias[0]/(float)accelsensitivity;
	dest2[1] = (float)accelBias[1]/(float)accelsensitivity;
	dest2[2] = (float)accelBias[2]/(float)accelsensitivity;
}


void MPU9250GetTemp(float *temp)
{
	uint16_t rawTemperature = 0;
	uint8_t tempHLByte[2] = {0};
#ifdef USE_SPI
	spiReadBytes(&spiDev, TEMP_OUT_H, tempHLByte, 2);
#else
	i2cReadBytes(&i2cDev, MPU9250_ADDR, TEMP_OUT_H, tempHLByte, 2);
#endif
	rawTemperature = ((uint16_t) (tempHLByte[0] << 8)) | ((uint16_t)tempHLByte[1]);
	*temp = ((float) rawTemperature) / 333.87f + 21.0f;
}


void MPU9250GetAccAxis(uint16_t *xAxis, uint16_t *yAxis, uint16_t *zAxis)
{
	uint8_t rawXYZ[6];
#ifdef USE_SPI
	spiReadBytes(&spiDev, ACCEL_XOUT_H, rawXYZ, 6);
#else
	i2cReadBytes(&i2cDev, MPU9250_ADDR, ACCEL_XOUT_H, rawXYZ, 6);
#endif
	*xAxis = (uint16_t)((uint16_t)rawXYZ[0] | (uint16_t)rawXYZ[1]);
	*yAxis = (uint16_t)((uint16_t)rawXYZ[2] | (uint16_t)rawXYZ[3]);
	*zAxis = (uint16_t)((uint16_t)rawXYZ[4] | (uint16_t)rawXYZ[5]);
}

void MPU9250GetGyroAxis(uint16_t *xAxis, uint16_t *yAxis, uint16_t *zAxis)
{
	uint8_t rawXYZ[6];
#ifdef USE_SPI
	spiReadBytes(&spiDev, GYRO_XOUT_H, rawXYZ, 6);
#else
	i2cReadBytes(&i2cDev, MPU9250_ADDR, GYRO_XOUT_H, rawXYZ, 6);
#endif
	*xAxis = (uint16_t)((uint16_t)rawXYZ[0] | (uint16_t)rawXYZ[1]);
	*yAxis = (uint16_t)((uint16_t)rawXYZ[2] | (uint16_t)rawXYZ[3]);
	*zAxis = (uint16_t)((uint16_t)rawXYZ[4] | (uint16_t)rawXYZ[5]);
}



