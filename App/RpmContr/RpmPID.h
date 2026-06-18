
#pragma once

#include "ControlBasics.h"
#include "EspMotor.h"

class RpmPID {
public:
  RateLim rl;
public:
  RpmPID(PIDParam* aPara) : rl(100)
  {
    rl.F_SAMPLE = 300.0; rl.SetRateSec(100);
    on = false;
    out = sum = 0;
    par = aPara;
  }

  void reset()
  {
    sum = 0;
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

  void calcOneStep(float aSensVal)
  {
    const float F_DIFF = 100.0;
    const float DT = 1.0 / 300.0;

    rl.CalcOneStep();
    float abw = rl.out - aSensVal;

    diff = abw - z3;
    z3 = z2; z2 = z1; z1 = abw;
    diff = diff * F_DIFF; 

    if (par->KI != 0) {
      if (fabs(rl.out) > 1E-3)
        sum += abw;
      else
        sum = 0;
    }

    out = par->KP * abw + par->KD * diff + sum * par->KI * DT;

    if (fabs(rl.out) < 50 && !rl.goesUp )
      out = 0;
  }
private:
  float x_n1;
  float sum;
  float z1, z2, z3;
public:
  float out;
  float diff;
  bool on;
  PIDParam* par;
};


PIDParam rpmPara(3.0E-3, 1.0E-4, 2.0E-2, 0.0, "rpm");

RpmPID rpmCL(&rpmPara), rpmCR(&rpmPara);

void ControlParams(int cmd)
{
  if (cmd == 200) {
    int num = ua0.ReadI16();
    rpmPara.ReadCOM(&ua0);
    rpmCL.rl.SetRateSec(rpmPara.rate);
    rpmCR.rl.SetRateSec(rpmPara.rate);
    ua0.SvMessage("Set PID");
  }
  if (cmd == 201) {
    int num = ua0.ReadI16();
    ua0.LockOStream();
    ua0.WrB(200);
    rpmPara.WriteCOM(&ua0);
    ua0.Flush();
    ua0.UnlockOStream();
    ua0.SvMessage("Get PID");
  }
}

void InitRpmPID()
{
  rpmPara.rate = 900;
  rpmCL.rl.SetRateSec(rpmPara.rate);
  rpmCR.rl.SetRateSec(rpmPara.rate);
}






