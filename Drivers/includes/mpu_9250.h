/*
 * mpu_9250.h
 *
 *  Created on: Mar 11, 2023
 *      Author: silvere
 */

#ifndef INCLUDES_MPU_9250_H_
#define INCLUDES_MPU_9250_H_

#include "global.h"

#define XG_OFFSET_H		0x13
#define XG_OFFSET_L		0x14
#define YG_OFFSET_H		0x15
#define YG_OFFSET_L		0x16
#define ZG_OFFSET_H		0x17
#define ZG_OFFSET_L		0x18

#define SMPLRT_DIV      0x19
#define CONFIG          0x1A
#define GYRO_CONFIG     0x1B
#define ACCEL_CONFIG    0x1C
#define ACCEL_CONFIG2   0x1D
#define LP_ACCEL_ODR	0x1E

#define FIFO_EN         0x23
#define I2C_MST_CTRL    0x24
#define INT_ENABLE      0x38
#define ACCEL_XOUT_H	0x3B
#define ACCEL_XOUT_L	0x3C
#define ACCEL_YOUT_H	0x3D
#define ACCEL_YOUT_L	0x3E
#define ACCEL_ZOUT_H	0x3F
#define ACCEL_ZOUT_L	0x40
#define TEMP_OUT_H		0x41
#define TEMP_OUT_L		0x42
#define GYRO_XOUT_H		0x43
#define GYRO_XOUT_L		0x44
#define GYRO_YOUT_H		0x45
#define GYRO_YOUT_L		0x46
#define GYRO_ZOUT_H		0x47
#define GYRO_ZOUT_L		0x48

#define USER_CTRL       0x6A
#define PWR_MGMT_2      0x6C
#define PWR_MGMT_1 		0x6B

#define FIFO_COUNTH     0x72
#define FIFO_COUNTL     0x73
#define FIFO_R_W        0x74

#define WHO_AM_I		0x75
#define XA_OFFSET_H     0x77
#define XA_OFFSET_L     0x78
#define YA_OFFSET_H     0x7A
#define YA_OFFSET_L     0x7B
#define ZA_OFFSET_H     0x7D
#define ZA_OFFSET_L     0x7E


#define MPU9250_ADDR 	(0x68 << 1)


// Set initial input parameters
enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum Gscale {
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};

uint8_t MPU9250WhoAmI(void);
void MPU9250Reset(void);
void MPU9250Init(void);
void MPU9250Calibrate(float * dest1, float * dest2);

void MPU9250GetTemp(float *temp);
void MPU9250GetAccAxis(uint16_t *xAxis, uint16_t *yAxis, uint16_t *zAxis);
void MPU9250GetGyroAxis(uint16_t *xAxis, uint16_t *yAxis, uint16_t *zAxis);


#endif /* INCLUDES_MPU_9250_H_ */
