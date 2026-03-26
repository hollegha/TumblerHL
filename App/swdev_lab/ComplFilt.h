#pragma once

#include <stdint.h>
#include <math.h>

extern MPU6050 mpu; // mpu rawData

class ComplFilt {
  // factor for grad/sec in 1/10 grad
  const float GY_ANGLE_FACT = (2000.0 / 32767.0) * 10.0;
  const float DELTA_T = 1E-3;
  const float ACC_MAX = 2050;
  const float RAD_GRAD = 180.0f / (float)M_PI;
  const float ALPHA = 0.005f; // 0.02
  const float BETA = 1.0f - ALPHA;
public:
  float omega; // in grad/sec
  int gyOffs;
  float gySum; // gyro angle through integration
  float accAngle;
  float complAngle;
  float vLen;
  Tp2OrdF accTp, gyroTp;
public:
  ComplFilt()
  {
    accTp.Init(TP_COE_p010); gyroTp.Init(TP_COE_p010);
    gyOffs = 0; omega = 0; accAngle = 0; complAngle = 0;
    vLen = 1.0;
  }

  void calGyro()
  {
    int sum = 0;
    for (int i = 0; i < 500; i++) {
      mpu.getGyroX(); // I2C rondtrip
      sum += mpu.gyro[0];
      esp_rom_delay_us(3000); // alle 3ms wert holen
    }
    gyOffs = sum / 500;
    // 500 gyro werte holen ( I2C )
    // ungefilterte Rohwerte
    // mittelwert auf gyOffs 
    // 500 werte mit 3ms dealy vom I2C mpu.getGyroX()
    // esp_rom_delay_us(3000);
  }

  void reset()
  {
    gySum = 0; complAngle = 0;
    // Integrator auf 0
  }

  void calcFilt(int accY, int gyX)
  {
    float gyCorr = gyX - gyOffs;
    omega = gyCorr * GY_ANGLE_FACT;
    gySum = gySum + omega * DELTA_T;
    vLen = sqrt(mpu.acc[0] * mpu.acc[0] + mpu.acc[1] * mpu.acc[1] + mpu.acc[2] * mpu.acc[2]);
    if (vLen < 1.0) vLen = 1.0;
    accAngle = 10.0 * RAD_GRAD * asin((float)accY / vLen); // in 1/10 grad
    complAngle = BETA * (complAngle + omega * DELTA_T) + ALPHA * accAngle;
  }
};
