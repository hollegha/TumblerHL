
#pragma once
#include "ControlBasics.h"

class BalanceController : public PIDParam {

public:
  BalanceController(float aKP, float aKD, float aKI, float aKS, const char* aName)
    : PIDParam(aKP, aKD, aKI, aKS, aName)
  {
    pow = 0; on = false; ang_1 = 0; diff = 0;
    angleOffs = -20.0; demPos = 0;
  }

  void CalcOneStep(float aAngle, float aSpeed, int actPos)
  {
    const float F_SAMP = 1000.0;
    aAngle += angleOffs;
    diff = (aAngle - ang_1) * F_SAMP;
    ang_1 = aAngle;

    float posVal = 0;;
    if (actPos > 20 || actPos < -20)
      // posVal = ClipHL(KI * (demPos - actPos),  0.3);
      posVal = ClipHL(KI * (actPos - demPos),  0.7);
    
    pow = KP * aAngle + KD * diff + KS * aSpeed + posVal;
  }
public:
  int demPos;
  float angleOffs;
public:
  float diff;
  float ang_1;
  float pow; // output to PWM
  bool on;
};

extern MPU6050 mpu;
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
Encoder2 encL(M2A, M2B), encR(M1A, M1B); // 2-Chan Encoder

RateLim limL(1000), limR(1000);

void InitIO()
{
  stdby.Init(); stdby.Set(1);
  Motor::InitTimer(); motR.Init(); motL.Init(); Motor::StartTimer();
  motR.setPow2(0.0); motL.setPow2(0.0);
  encL.Init(); encR.Init(); encR.inv=true;
  printf("InitIO finished\n");
}


