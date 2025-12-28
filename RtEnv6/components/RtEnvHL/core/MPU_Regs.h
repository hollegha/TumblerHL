
#pragma once

#define MPU_ADDRESS 0x68 // address pin low (GND), default for InvenSense evaluation board

#define MPU_CONFIG_REG         0x1A
#define MPU_GYRO_CONFIG_REG    0x1B
#define MPU_ACCELERO_CONFIG_REG    0x1C
  
#define MPU_INT_PIN_CFG        0x37

#define MPU_ACCEL_XOUT_H_REG   0x3B
#define MPU_ACCEL_YOUT_H_REG   0x3D
#define MPU_ACCEL_ZOUT_H_REG   0x3F

// Hier ist das GetAll()       0x40 
#define MPU_TEMP_H_REG         0x41
 
#define MPU_GYRO_XOUT_H_REG    0x43
#define MPU_GYRO_YOUT_H_REG    0x45
#define MPU_GYRO_ZOUT_H_REG    0x47

#define MPU_PWR_MGMT_1_REG     0x6B
#define MPU_WHO_AM_I_REG       0x75

#define MPU_SLP_BIT    6
#define MPU_BYPASS_BIT 1

#define MPU_BW_256  0
#define MPU_BW_188  1
#define MPU_BW_98   2
#define MPU_BW_42   3
#define MPU_BW_20   4
#define MPU_BW_10   5
#define MPU_BW_5    6

#define MPU_ACCELERO_RANGE_2G   0
#define MPU_ACCELERO_RANGE_4G   1
#define MPU_ACCELERO_RANGE_8G   2
#define MPU_ACCELERO_RANGE_16G  3

#define MPU_GYRO_RANGE_250      0
#define MPU_GYRO_RANGE_500      1
#define MPU_GYRO_RANGE_1000     2
#define MPU_GYRO_RANGE_2000     3

#define INT_PIN_CFG 0x37
#define INT_ENABLE 0x38
#define DMP_INT_STATUS 0x39 // Check DMP interrupt
#define INT_STATUS 0x3A
















