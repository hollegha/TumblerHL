
#include "RTEnvHL.h"
#include "EspMotor.h"
// #include "SvProtocol3.h"
#include "FloatFilt.h"
#include "Features.h"

PwmTim RcServo::tim;
PwmTim Motor::tim;

void Motor::InitTimer()
{
  ChkFeature2(FEAT_SEM);
  tim = InitPwmTimer(0, 1000000, 1000);
}

void Motor::StartTimer()
{
  ChkFeature2(FEAT_SEM);
  ::StartPwmTimer(tim);
}

void Motor::Init()
{
  ChkFeature2(FEAT_SEM);
  hpwm = InitPwm(tim, _pwm);
  GpIoInit(1ULL << _fwd);
  if (_rev != -1)
    GpIoInit(1ULL << _rev);
}


static void IRAM_ATTR encoder_isr(void* arg)
{
	((Encoder*)arg)->ISRFunction();
}

static void IRAM_ATTR encoder_isr2(void* arg)
{
  ((Encoder*)arg)->ISRFunction2();
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

Encoder::Encoder(int aInt, int aDir, Motor* aMot)
{
  _intrPin = aInt; _dirPin = aDir; inv = false;
  dirFlag=&(aMot->dir); actDir=*dirFlag;
  cnt = 0; _t1 = 0; pw = 0x0FFFFFF;
  _runFlag = 0; ignCnt = 0; filtOn = true;
  z1 = z2 = z3 = diff = 0; info = 0;
  _filt = new Tp2OrdF();
  _filt->Init(TP_COE_p010);
}

void Encoder::Init()
{
  ChkFeature2(FEAT_SEM);
  if (_dirPin != -1) {
    GpIoInit(1ULL << _dirPin, true);
    GpIoInitInterrupt(_intrPin, encoder_isr, this, GPIO_INTR_POSEDGE);
  }
  else {
    GpIoInitInterrupt(_intrPin, encoder_isr2, this, GPIO_INTR_POSEDGE);
  }
}

void Encoder::setBW(int aFiltNum)
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


void Encoder::ISRFunction()
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

void Encoder::CalcFilt2()
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

float Encoder::getFrequF()
{
  if (!filtOn)
    return getFrequ();
  return _filt->yn;
}

void Encoder::checkDir()
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


// min 800us
void Encoder::ISRFunction2()
{
  const uint32_t MIN_PW = 600;
  const uint32_t MAX_PW = 17000;
  const uint32_t MAX_PW_DIFF = 500; // 59 Hz
  
  _runFlag++;
  int64_t t2 = esp_timer_get_time();
  uint32_t actPw = t2 - _t1;
  _t1 = t2;

  if (ignCnt>3) {
    ignCnt = 0;
    goto UseMeasurement;
    return;
  }
  
  if (AbsDiff(pw, actPw)>MAX_PW_DIFF) {
    ignCnt++;
    return;
  } 
  
  // else the main-case
UseMeasurement:
  if( actPw>MIN_PW && actPw<MAX_PW )
    { pw = actPw; cnt++; }
}

void Encoder::CalcFilt()
{
  int64_t actPw = esp_timer_get_time() - _t1;
  if (actPw > 50000 && _runFlag != 0) {
    _runFlag = 0;
    pw = 0x0FFFFFF;
  }
  if( *dirFlag!=actDir && getFrequUS()<80 )
    actDir = *dirFlag;
  if (actDir > 0)
    _filt->CalcOneStep((1.0 / (float)pw) * FREQU_FACT);
  else // <=0
    _filt->CalcOneStep((-1.0 / (float)pw) * FREQU_FACT);
}

void Encoder::CalcDiff(float aVal)
{
  diff = aVal - z3;
  z3 = z2; z2 = z1; z1 = aVal;
}












