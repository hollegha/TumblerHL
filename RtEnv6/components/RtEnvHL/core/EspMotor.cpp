
#include "RTEnvHL.h"
#include "EspMotor.h"
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

void Motor::setPow2(float aPow)
{
  if (aPow > 0.95) aPow = 0.95;
  if (aPow < -0.95) aPow = -0.95;
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

void Motor::setDirPin(bool dir)
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


void RateLim::CalcOneStep()
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













