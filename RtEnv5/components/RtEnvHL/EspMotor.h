
#pragma once

#include "TimerHL.h"
#include "math.h"


class RcServo {
public:
  int offset;
  int pw;
public:
  static void InitTimer()
  {
    tim=InitPwmTimer(0, 1000000, 20000);
  }
  static void StartTimer()
  {
    ::StartPwmTimer(tim);
  }

  RcServo(int aPin)
  {
    _pin = aPin;
    offset=1300; pw=0;
  }
  void Init()
  {
    hpwm=InitPwm(tim, _pin);
  }
  void SetPw(uint32_t width)
  {
    pw = width;
    ::SetPw(hpwm,offset+width);
  }
  void SetPercent(float aVal)
  {
    SetPwPercent(hpwm, aVal);
  }
private:
  int _pin;
  PwmHL hpwm;
  static PwmTim tim;
};


class Motor {
public:
  float pow;
  float dir;
  bool inv;
public:
  static void InitTimer();
  static void StartTimer();
  Motor(int aPwm, int aFwd, int aRev=-1)
  {
    _pwm = aPwm; _fwd = aFwd; _rev = aRev;
    pow = 0; dir = 0; inv = false;
  }
  void Init();
  void setPow(float aPow)
  {
    if (_rev == -1)
      return;
    if (aPow > 0) {
      gpio_set_level((gpio_num_t)_fwd, 1);
      gpio_set_level((gpio_num_t)_rev, 0);
      dir = true;
    }
    else {
      aPow = -aPow;
      gpio_set_level((gpio_num_t)_fwd, 0);
      gpio_set_level((gpio_num_t)_rev, 1);
      dir = false;
    }
    SetPwPercent(hpwm, aPow);
  }
  // Tumbler Motor
  void setPow2(float aPow)
  {
    if (aPow > 0.9) aPow = 0.9;
    if (aPow < -0.9) aPow = -0.9;
    pow = aPow;
    if (aPow >= 0) {
      setDirPin(false);
    }
    else {
      aPow = -aPow;
      setDirPin(true);
    }
    SetPwPercent(hpwm, aPow);
    if (pow > 0) dir = 1.0;
    else if (pow < 0) dir = -1.0;
    else dir = 0.0;
  }
  void setDirPin(bool dir)
  {
    if( dir )
      if( !inv )
        gpio_set_level((gpio_num_t)_fwd, 1);
      else
        gpio_set_level((gpio_num_t)_fwd, 0);
    else
      if (!inv)
        gpio_set_level((gpio_num_t)_fwd, 0);
      else
        gpio_set_level((gpio_num_t)_fwd, 1);
  }
private:
  int _pwm, _fwd, _rev;
  PwmHL hpwm;
  static PwmTim tim;
};

class Tp2OrdF;

class Encoder {
  const float FREQU_FACT = 1E6;
  const float DT_FACT = 150.0;
public:
  Encoder(int aInt, int aDir, Motor* aMot);
  void Init();
  void ISRFunction();
  // Tumbler
  void ISRFunction2();
  void setBW(int aFiltNum); // 1..small -> 5..big 0..off

  // 1-Chan Encoder
  void CalcFilt();

  // 2-Chan Encoder
  void CalcFilt2();

  void CalcDiff(float aVal);
  // unsigned
  float getFrequUS() 
  {
    return (1.0 / (float)pw) * FREQU_FACT;
  }
  // signed
  float getFrequ() 
  {
    if (actDir > 0)
      return (1.0 / (float)pw) * FREQU_FACT;
    else if (actDir < 0)
      return (-1.0 / (float)pw) * FREQU_FACT;
    else
      return 0.0;
  }
  // filtert
  float getFrequF(); 
  float getDiff()
  {
    return diff * DT_FACT;
  }
  int getPW()
  {
    if (pw > 32000)
      return 32000;
    else
      return pw;
  }
  int getDir()
  {
    return actDir;
  }
  void syncDir()
  {
    actDir = *dirFlag;;
  }
  void checkDir();
private:
  int _dirPin, _intrPin;
  float actDir, *dirFlag;
  bool filtOn;
  int64_t _t1;
  uint32_t _runFlag, ignCnt;
  Tp2OrdF* _filt;
  float z1, z2, z3;
public:
  uint32_t cnt;
  uint32_t pw;
  float diff;
  bool inv;
  int info;
};


class RateLim {
public:
  float F_SAMPLE;
public:
  float inTmp;
  float in, out;
  int16_t goesUp;
  int16_t on;
public:
  RateLim(float aRate)
  {
    F_SAMPLE = 300.0;
    in = out = 0; on = 1; SetRateSec(aRate);
  }
  void SaveIn()
  {
    inTmp = in;
  }
  void RestoreIn()
  {
    in = inTmp;
  }
  int IsFlat()
  {
    return out == in;
  }
  void Reset()
  {
    in = out = 0;
  }
  // 1000V in 300 Ticks = 1000/300
  void SetRateTicks(float aRate)
  {
    on = 1;
    if (aRate == 0) on = 0;
    _rate = aRate;
  }
  // eg. 1000     = 1000V/Sec
  // eg. 1000/10  = 1000V/10Sec
  // e.g 1000/0.1 = 1000V/0.1Sec
  void SetRateSec(float aRate)
  {
    on = 1;
    if (aRate == 0) on = 0;
    _rate = aRate / F_SAMPLE;
  }
  void CalcOneStep()
  {
    goesUp = 0;
    if (!on) {
      out = in; return;
    }
    if (fabs(out - in) <= _rate) {
      out = in; return;
    }
    if (in > 0 && in > out)
      goesUp = 1;
    if (in < 0 && in < out)
      goesUp = 1;
    if (in > out)
    {
      out += _rate;
    }
    else
    {
      out -= _rate;
    }
  }
public:
  float _rate;
};










