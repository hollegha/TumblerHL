
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "LineSensor.h"


void LineSensor::initADC()
{
  /* lsAry[0].adc._ch = ADC_CHANNEL_0;
  lsAry[1].adc._ch = ADC_CHANNEL_3;
  lsAry[2].adc._ch = ADC_CHANNEL_6;
  lsAry[3].adc._ch = ADC_CHANNEL_7;
  lsAry[4].adc._ch = ADC_CHANNEL_4;
  lsAry[5].adc._ch = ADC_CHANNEL_5; */
  lsAry[0].adc._ch = ADC_CHANNEL_0;
  lsAry[1].adc._ch = ADC_CHANNEL_3;
  lsAry[2].adc._ch = ADC_CHANNEL_4;
  lsAry[3].adc._ch = ADC_CHANNEL_5;
  lsAry[4].adc._ch = ADC_CHANNEL_6;
  lsAry[5].adc._ch = ADC_CHANNEL_7;
  for (int i = 0; i < N_LS_CHAN; i++) {
    lsAry[i].adc.Init();
  }
}

void LineSensor::setDefaultCalib()
{
  setCal(0, 64, 2770);
  setCal(1, 52, 2070);
  setCal(2, 48, 1862);
  setCal(3, 51, 2260);
  setCal(4, 45, 2044);
  setCal(5, 59, 2713);
}

void LineSensor::calcPos()
{
  int sum = 0, weight = 0, i;
  posDiff = 0; // 1000
  if (lsAry[0].y < 1200 && lsAry[5].y < 1200 && isMidZero()) {
    return;
  }
  for (i = 0; i < N_LS_CHAN; i++)
    sum += lsAry[i].y;
  if (sum == 0)
    sum = 1;
  for (i = 0; i < N_LS_CHAN; i++)
    weight += CAL_AMPL * (i + 1) * lsAry[i].y;
  pos = (weight / sum) - 7000;
}

// tuning of Line-Position
const int RG_MIN = 20;
const int RG_MAX = 1700; // 1800 2000

void LineSensor::calcPos3()
{
  if (lsAry[0].y < 1200 && lsAry[5].y < 1200 && isMidZero()) {
    return;
  }
  if (checkRange(0, 1))
    pos3 = pos2 - 4 * RG_MAX;
  else if (checkRange(1, 2))
    pos3 = pos2 - 2 * RG_MAX;
  else if (checkRange(2, 3))
    pos3 = pos2;
  else if (checkRange(3, 4))
    pos3 = pos2 + 2 * RG_MAX;
  else if (checkRange(4, 5))
    pos3 = pos2 + 4 * RG_MAX;

  tp.CalcOneStep(pos3);
  posDiff = tp.y - z2;
  z2 = z1; z1 = tp.y;
  posDiff = posDiff * (1.0 / 3.0);
}

bool LineSensor::checkRange(int idxL, int idxR)
{
  if (lsAry[idxL].y < RG_MIN && lsAry[idxR].y < RG_MAX)
    return false;
  if (lsAry[idxL].y < RG_MAX && lsAry[idxR].y < RG_MIN)
    return false;
  pos2 = lsAry[idxR].y - lsAry[idxL].y;
  return true;
}


// links:0 rechts:5 links:- rechts:+
void LineSensor::calcPos2()
{
  if (lsAry[2].y < RG_MIN && lsAry[3].y < RG_MAX)
    goto outOfRange;
  if (lsAry[2].y < RG_MAX && lsAry[3].y < RG_MIN)
    goto outOfRange;
  pos2 = lsAry[3].y - lsAry[2].y;
  return;
outOfRange:
  if (pos2 < 0)
    pos2 = -CAL_AMPL;
  else
    pos2 = CAL_AMPL;
}


