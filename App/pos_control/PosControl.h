
#pragma once
#include "ControlBasics.h"

class PosController : public PIDParam {
public:
  RateLim rl;
public:
  PosController(float aRate, float aKP, float aKD, const char* aName)
    : PIDParam(aKP, aKD, 0, 0, aName), rl(aRate)
  {
    rl.F_SAMPLE = 100.0; rl.SetRateSec(aRate);
    pow = 0; on = false; x_1 = 0; diff = 0;
  }
  PosController(PosController& aPid) : rl(100.0)
  {
    rl.F_SAMPLE = 100.0; rl._rate = aPid.rl._rate;
    Copy(aPid);
    pow = 0; on = false; x_1 = 0; diff = 0;
  }
  void setDemand(int aVal)
  {
    rl.in = aVal;
  }
  float getActDemand()
  {
    return rl.out;
  }
  void setRate(float aRate)
  {
    rl.SetRateSec(aRate);
  }

  void CalcOneStep(int actPos)
  {
    const float F_DIFF = 10.0;
    rl.CalcOneStep();
    float abw = rl.out - actPos;
    diff = (abw - x_1)*F_DIFF; x_1 = abw;
    pow = KP*abw + KD*diff;
  }
public:
  float diff;
  float pow; // output to PWM
  bool on;
private:
  float x_1;
};

