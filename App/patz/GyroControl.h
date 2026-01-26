
#pragma once

#include "MPU_Esp.h"
#include "ImuAlgo.h"
#include "ControlBasics.h"

extern MPU6050 mpu;
ImuChan gyro(mpu.gyro, 2);
KalmSimple integ;

class AngleController : public PIDParam {
public:
  AngleController(float aKP, float aKD, const char* aName)
    : PIDParam(aKP, aKD, 0.0, 0.0, aName)
  {
    on = false;
    demand = out = x_n1 = diff = abw = 0;
  }

  void calcOneStep(float aSensVal)
  {
    const float F_SAMP = 1E3;
    abw = demand - aSensVal;
    diff = abw - x_n1; x_n1 = abw;
    diff = diff * F_SAMP;
    out = KP * abw + KD * diff;
  }
private:
  float x_n1;
public:
  float demand; // sollwert
  float out;
  float diff;
  float abw;
};

AngleController rgl(3.0E-2, 3.0E-4, "gyroC");

void ControlParams(int cmd)
{
  if (cmd == 200) {
    int num = ua0.ReadI16();
    rgl.ReadCOM(&ua0);
    ua0.SvMessage("Set PID");
  }
  if (cmd == 201) {
    int num = ua0.ReadI16();
    ua0.LockOStream();
    ua0.WrB(200);
    rgl.WriteCOM(&ua0);
    ua0.Flush();
    ua0.UnlockOStream();
    ua0.SvMessage("Get PID");
  }
}




