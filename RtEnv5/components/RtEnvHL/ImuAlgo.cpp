
#include "ImuAlgo.h"
#include "MPU_Esp.h"
#include "FloatFilt.h"
// esp_rom_delay_us()
#include "freertos/FreeRTOS.h"
#include "math.h"

extern MPU6050 mpu;

void ImuChan::CalGyro()
{
  int sum[3], i;
  for (i = 0; i < 3; i++) {
    sum[i] = 0;
    this->offset[i] = 0;
  }
  
  for (int nCal = 1; nCal <= 300; nCal++) {
    esp_rom_delay_us(3000);
    mpu.getGyro(); this->CalcFilt();
    for (i = 0; i < 3; i++)
      sum[i] += this->getFilt2(i);
  }

  for (i = 0; i < 3; i++)
    this->offset[i] = sum[i] / 300;
}

ImuChan::ImuChan(int* aValAry, int type)
{
  ary = aValAry;
  int i;
  for (i = 0; i < 3; i++) {
    offset[i] = 0;
    f1[i] = new Tp1Ord();
    f2[i] = new Tp2OrdF();
  }
  for (i = 0; i < 3; i++) {
    if( type==1 )
      f2[i]->Init(TP_COE_p010); // acc
    else
      f2[i]->Init(TP_COE_p05); // gyro
  }
}

void ImuChan::SetAlpha(float aAlpha)
{
  for(int i=0; i<3; i++)
    f1[i]->SetAlpha(aAlpha);
}

void ImuChan::CalcFilt()
{
  for (int i = 0; i < 3; i++) {
    f1[i]->CalcOneStep(ary[i]);
    f2[i]->CalcOneStep(ary[i]);
  }
}

void ImuChan::CalcFilt(int idx)
{
  f1[idx]->CalcOneStep(ary[idx]);
  f2[idx]->CalcOneStep(ary[idx]);
}

int ImuChan::getVal(int idx)
{
  return ary[idx] - offset[idx];
}

float ImuChan::getFilt1(int idx)
{
  return f1[idx]->y - (float)offset[idx];
}

float ImuChan::getFilt2(int idx)
{
  return f2[idx]->yn - (float)offset[idx];
}


const float DT_SENS = 1E-3;
const float ACC_MAX = 2050; // -2000..2105
const float GY_ANGLE_FACT = (2000.0 / 32767.0) * 10.0;
const float ALPHA = 0.001f; // 0.02
const float BETA = 1.0f - ALPHA;
const float RAD_GRAD = 180.0f/(float)M_PI;

void KalmSimple::CalcFilter(float accVal, float gyroVal)
{
  if (accVal > ACC_MAX) accVal = ACC_MAX;
  if (accVal < -ACC_MAX) accVal = -ACC_MAX;
  accAngle = 10.0f * RAD_GRAD * asin((float)accVal / (float)ACC_MAX);
  omega = GY_ANGLE_FACT * DT_SENS * gyroVal;
  
  gySum += omega;
  complAngle = BETA*(complAngle + omega) + ALPHA*accAngle;
}







