
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
  bool  inv;
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
    if (dir)
      if (!inv)
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

class Encoder {
public:
  Encoder(int aInt, int aDir, Motor* aMot);
  void Init();
  void ISRFunction();
  // Tumbler
  void ISRFunction2();
  void checkDir();
private:
  int _dirPin, _intrPin;
public:
  int32_t cnt;
  bool inv;
};










