
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "LineSensor.h"
#include "NodeLock.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "LedStripHL.h"

extern nvs_handle_t nvsRtEnv; 

// Ls0 == links von vorne gesehen


void LineSensor::cal2nvs()
{
  int calBlob[12];
  int j = 0;
  for (int i = 0; i < 6; i++) {
    calBlob[j] = lsAry[i]._max; j++;
    calBlob[j] = lsAry[i]._min; j++;
  }
  openRtEnvStore();
  ESP_ERROR_CHECK(nvs_set_blob(nvsRtEnv, "ls", calBlob, 4*12));
  ESP_ERROR_CHECK(nvs_commit(nvsRtEnv));
  closeRtEnvStore();
}

int LineSensor::nvs2cal()
{
  int calBlob[12];
  size_t len; int ret = 0;
  openRtEnvStore();
  ret = nvs_get_blob(nvsRtEnv, "ls", calBlob, &len);
  closeRtEnvStore();
  if (ret != ESP_OK)
    return ret;
  int j = 0;
  for (int i = 0; i < 6; i++) {
    lsAry[i]._max = calBlob[j]; j++;
    lsAry[i]._min = calBlob[j]; j++;
    lsAry[i].cal2_kd();
  }
  return ESP_OK;
}

void LineSensor::dispOnLeds()
{
  leds.clear();
  for (int i = 0; i < 6; i++) {
    if (Y(i) > 50.0)
      leds.setAmplitude(i + 1, Y(i) * (1.0 / CAL_AMPL));
  }
  leds.refresh();
}


void LsPololu::initADC()
{
  printf("LsPolo N:%d\n", N_LS_CHAN);
  // Sesor in front
  lsAry[0].adc._ch = ADC_CHANNEL_0;
  lsAry[1].adc._ch = ADC_CHANNEL_3;
  lsAry[2].adc._ch = ADC_CHANNEL_4;
  lsAry[3].adc._ch = ADC_CHANNEL_5;
  lsAry[4].adc._ch = ADC_CHANNEL_6;
  lsAry[5].adc._ch = ADC_CHANNEL_7; 
  // Sensor back
  /* lsAry[0].adc._ch = ADC_CHANNEL_7;
  lsAry[1].adc._ch = ADC_CHANNEL_6;
  lsAry[2].adc._ch = ADC_CHANNEL_5;
  lsAry[3].adc._ch = ADC_CHANNEL_4;
  lsAry[4].adc._ch = ADC_CHANNEL_3;
  lsAry[5].adc._ch = ADC_CHANNEL_0; */
  for (int i = 0; i < N_LS_CHAN; i++) {
    lsAry[i].adc.Init();
  }
}

void LsBertl::initADC()
{
  printf("LsB N:%d\n", N_LS_CHAN);
  lsAry[0].adc._ch = ADC_CHANNEL_6;
  lsAry[1].adc._ch = ADC_CHANNEL_0;
  lsAry[2].adc._ch = ADC_CHANNEL_3;
  lsAry[3].adc._ch = ADC_CHANNEL_7;
  for (int i = 0; i < N_LS_CHAN; i++) {
    lsAry[i].adc.Init();
  }
}


void LsPololu::setDefaultCalib()
{
  setCal(0, 32, 3081);
  setCal(1, 21, 2640);
  setCal(2, 19, 2437);
  setCal(3, 24, 2561);
  setCal(4, 34, 2656);
  setCal(5, 35, 3211); 
}

void LsBertl::setDefaultCalib()
{
  setCal(0, 25, 3540);
  setCal(1, 17, 3272);
  setCal(2, 44, 3607);
  setCal(3, 32, 3658);
}



// tuning of Line-Position
const int RG_MIN = 20;
const int RG_MAX = 1500; // 1800 2000 1700

void LsPololu::calcPos()
{
  if (lsAry[0].y < 1200 && lsAry[5].y < 1200 && isMidZero()) {
    return;
  }
  if (checkRange(0, 1))
    _pos = _rgPos - 4 * RG_MAX;
  else if (checkRange(1, 2))
    _pos = _rgPos - 2 * RG_MAX;
  else if (checkRange(2, 3))
    _pos = _rgPos;
  else if (checkRange(3, 4))
    _pos = _rgPos + 2 * RG_MAX;
  else if (checkRange(4, 5))
    _pos = _rgPos + 4 * RG_MAX;

  tp.CalcOneStep(_pos);
  _posDiff = tp.y - z2;
  z2 = z1; z1 = tp.y;
  _posDiff = _posDiff * (1.0 / 3.0);
}

void LsPololu::calcPos2()
{
  if (lsAry[0].y < 1200 && lsAry[5].y < 1200 && isMidZero()) {
    return;
  }

  float sum = 0;
  for (int i = 0; i < N_LS_CHAN; i++)
    sum += Y(i);
  if (sum == 0) sum = 1;

  float weight = 0;
  for (int i = 0; i < N_LS_CHAN; i++)
    weight += Y(i) * (i * RG_MAX);

  _pos = (weight / sum) - 2.5 * RG_MAX;
}

bool LsPololu::checkRange(int idxL, int idxR)
{
  if (lsAry[idxL].y < RG_MIN && lsAry[idxR].y < RG_MAX)
    return false;
  if (lsAry[idxL].y < RG_MAX && lsAry[idxR].y < RG_MIN)
    return false;
  _rgPos = lsAry[idxR].y - lsAry[idxL].y;
  return true;
}


void LsBertl::experPos()
{
  const int RG_OFFSET = 2000;
  const int MID_OFFSET = 1200; // könnte man kalibrieren
  _pos = 0;
  if (Y(1) < 50 && Y(2) < 50 && Y(0) < 1800 && Y(3) < 1800)
    return; // out of Range xxx
  
  _pos = Y(1) - Y(0) - 2 * RG_OFFSET;
  
  if (Y(2) > 300) 
    _pos = Y(2) - Y(1);

  if (Y(3) > 300)
    _pos = Y(3) - Y(2) + 2 * RG_OFFSET;

  _pos -= MID_OFFSET;
}

