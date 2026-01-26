
#include "RTEnvHL.h"
#include "FloatFilt.h"
#include "Features.h"
#include "Encoder2.h"


static void IRAM_ATTR encoder_isr(void* arg)
{
	((Encoder2*)arg)->ISRFunction();
}

inline int AbsDiff(uint32_t a, uint32_t b)
{
  int diff = a - b;
  if (diff < 0)
    return -diff;
  else
    return diff;
}

inline bool IsFrequLow(uint32_t aPw)
{
  // 50000 20Hz
  // 25000) 40Hz
  if (aPw > 5000) // 200Hz
    return true;
  return false;
}

inline bool Is_F_gt_500(uint32_t aPw)
{
  if (aPw < 2000) // 500Hz
    return true;
  return false;
}

inline int frequDiff(uint32_t pw1, uint32_t pw2)
{
  const uint32_t FREQU_FACT_I = 1E6;
  int diff = (int)(FREQU_FACT_I / pw1) - (int)(FREQU_FACT_I / pw2);
  if (diff < 0)
    return -diff;
  return diff;
}


Encoder2::Encoder2(int aInt, int aDir)
{
  _intrPin = aInt; _dirPin = aDir; inv = false;
  actDir = 1;
  cnt = 0; _t1 = 0; pw = 0x0FFFFFF;
  _runFlag = 0; filtOn = true;
  z1 = z2 = z3 = diff = 0; info = 0;
  _filt = new Tp2OrdF();
  _filt->Init(TP_COE_p010);
}

void Encoder2::Init()
{
  // printf("Enc2 init\n");
  ChkFeature2(FEAT_SEM);
  GpIoInit(1ULL << _dirPin, true);
  GpIoInitInterrupt(_intrPin, encoder_isr, this, GPIO_INTR_POSEDGE);
}

void Encoder2::setBW(int aFiltNum)
{
  filtOn = true;
  if (aFiltNum<1 || aFiltNum>5) {
    filtOn = false; return;
  }
  switch (aFiltNum) {
  case 1:
    _filt->Init(TP_COE_p010); break;
  case 2:
    _filt->Init(TP_COE_p025); break;
  case 3:
    _filt->Init(TP_COE_p04); break;
  case 4:
    _filt->Init(TP_COE_p05); break;
  case 5:
    _filt->Init(TP_COE_p100); break;
  default:
    _filt->Init(TP_COE_p010);
  }
}


void Encoder2::ISRFunction()
{
  _runFlag = 1;
  int64_t t2 = esp_timer_get_time();
  int64_t actPw = t2 - _t1;
  _t1 = t2;
  
  info = 0;
  if (IsFrequLow(pw)) {
    info = 200;
    if (Is_F_gt_500(actPw))
      return;
  }
  
  if (actPw < 600) // ignore short pw
    return;
  pw = actPw;
  checkDir();
}

void Encoder2::CalcFilt2()
{
  int64_t actPw = esp_timer_get_time() - _t1;
  if (actPw > 50000 && _runFlag != 0) {
    _runFlag = 0; // _filt->Reset();
    pw = 0x0FFFFFF;
  } 
  if (actDir > 0)
    _filt->CalcOneStep((1.0 / (float)pw) * FREQU_FACT);
  else // <=0
    _filt->CalcOneStep((-1.0 / (float)pw) * FREQU_FACT);
  /* if (getFrequUS() < 10) {
    _filt->Reset();
  } */
}

float Encoder2::getFrequF()
{
  if (!filtOn)
    return getFrequ();
  return _filt->yn;
}

void Encoder2::checkDir()
{
  if (!inv) {
    if (gpio_get_level((gpio_num_t)_dirPin)) {
      cnt++; actDir = 1;
    }
    else {
      cnt--; actDir = -1;
    }
  }
  else {
    if (gpio_get_level((gpio_num_t)_dirPin)) {
      cnt--; actDir = -1;
    }
    else {
      cnt++; actDir = 1;
    }
  }
}


void Encoder2::CalcDiff(float aVal)
{
  diff = aVal - z3;
  z3 = z2; z2 = z1; z1 = aVal;
}












