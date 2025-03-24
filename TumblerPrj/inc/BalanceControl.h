
#pragma once
#include "math.h"

template<class T>
class RangeClip {
public:
  T min, max;
public:
  RangeClip(T aMin, T aMax)
  {
    min = aMin; max = aMax;
  }

  T Clip(T aVal)
  {
    if (aVal > max)
      return max;
    if (aVal < min)
      return min;
    return aVal;
  }
};


class PIDParam {
public:
  PIDParam(float aKP, float aKD, float aKI, float aKS, const char* aName)
  {
    KP = aKP; KD = aKD; KI = aKI; on = 0; _name = aName;
    KS = aKS;
  }

  PIDParam(PIDParam& aPid)
  {
    Copy(aPid); on = 0;
  }

  void Copy(PIDParam& aPid)
  {
    KP = aPid.KP; KD = aPid.KD; KI = aPid.KI; _name = aPid._name;
    KS = aPid.KS;
  }

  void ReadCOM(SvProtocol2* aP)
  {
    KP = aP->ReadF(); KD = aP->ReadF();
    KI = aP->ReadF(); KS = aP->ReadF();
  }

  void WriteCOM(SvProtocol2* aP)
  {
    aP->SvMessage(_name);
    aP->WrF(KP).WrF(KD).WrF(KI).WrF(KS);
  }

  void Print(SvProtocol2* aP)
  {
    aP->SvPrintf("%s %1.1f %1.1f %1.1f %1.1f", _name, KP, KD, KI, KS);
  }
public:
  int16_t on;
  float KP, KD, KI, KS;
  const char* _name;
};


class BalanceController : public PIDParam {
  
public:
  BalanceController(float aKP, float aKD, float aKI, float aKS, const char* aName)
    : PIDParam(aKP, aKD, aKI, aKS, aName)
  {
    pow = 0; on = false; ang_1 = 0; diff = 0;
  }

  void CalcOneStep(float aAngle, float aSpeed)
  {
    const float F_SAMP = 1000.0;
    diff = (aAngle - ang_1)*F_SAMP;
    ang_1 = aAngle;

    // if (fabs(aSpeed) < 60)
    // pow = KP * aAngle + KD * diff;
    pow = KP*aAngle + KD*diff + KS*aSpeed;
  }

public:
  float diff;
  float ang_1;
  float pow; // output to PWM
  bool on;
};



