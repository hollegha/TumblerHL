#pragma once

SemaphoreHandle_t sem1 = 0;

MPU6050 mpu;
ImuChan imuAcc(mpu.acc, 1);
ImuChan imuGyro(mpu.gyro, 2);
KalmSimple kalm;
int dispMode = 1;
bool calFlag = false;

const int PWA = 4, AI1 = 32, PWB = 33, BI1 = 5, ST_BY = 18;
const int M2A = 19, M1A = 23, M2B = 36, M1B = 39;

Motor motL(PWA, AI1, -1); // -1==Tumbler Motor
Motor motR(PWB, BI1, -1);
GpIoOut stdby(ST_BY);
Encoder encL(M2A, -1, &motL), encR(M1A, -1, &motR); // 1-Chan Encoder
// Encoder encL(M2A, M2B, &motL), encR(M1A, M1B, &motR); // 2-Chan Encoder

RateLim limL(1000), limR(1000);

void InitIO()
{
  sem1 = xSemaphoreCreateMutex();
  stdby.Init(); stdby.Set(1);
  Motor::InitTimer(); motR.Init(); motL.Init(); Motor::StartTimer();
  motR.setPow2(0.0); motL.setPow2(0.0);
  encL.Init(); encR.Init(); encR.inv=true;
  printf("InitIO finished\n");
}

inline void LockOStream()
{
  xSemaphoreTake(sem1, portMAX_DELAY);
}

inline void UnlockOStream()
{
  xSemaphoreGive(sem1);
}

