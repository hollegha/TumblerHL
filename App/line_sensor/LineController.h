
#pragma once
#include "LineSensor.h"
#include "ControlBasics.h"
#include "TTMailBox.h"

class LinePID : public PIDParam {
public:
  float out;
  float forew; // foreward speed
public:
  LinePID(float aKP, float aKD, float aKI, float aKS, const char* aName) :
    PIDParam(aKP, aKD, aKI, aKS, aName)
  {
    out = forew = sum = 0;
  }
  void calcOneStep();
  void onOff(bool aOn)
  {
    on = aOn;
    sum = 0;
  }
private:
  float sum;
};

extern LsPololu ls;
extern LinePID lpd;
extern SvProtocol3 ua0;
extern TTMailBox drCmd;

void InitLineControler();

