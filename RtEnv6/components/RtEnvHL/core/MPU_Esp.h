
#pragma once

void I2cInit();

void initMPU(); // calls I2cInit()

class MPU6050 {
public:
  int acc[3];   // acc raw vals
  int gyro[3];  // gyro raw vals
public:
  MPU6050();
  
  int testConnection();
  void Init();

  void setBW(char BW);
  void setI2CBypass(bool state);

  void setAccelRange(char range);
  
  int getAccelX();
  int getAccelY();
  int getAccelZ();
  void getAccel();

  void setGyroRange(char range);
  int getGyroX();
  int getGyroY();
  int getGyroZ();
  void getGyro();

  int getTempRaw(void);
  float getTemp(void);
  
  int CheckReadyBit();
  
  void setSleepMode(bool state);
};

extern MPU6050 mpu;

