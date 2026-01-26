
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
  void setPow2(float aPow);
  void setDirPin(bool dir);
private:
  int _pwm, _fwd, _rev;
  PwmHL hpwm;
  static PwmTim tim;
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
  void CalcOneStep();
public:
  float _rate;
};

// motor and encoder are one thing for the user
#include "Encoder2.h"
