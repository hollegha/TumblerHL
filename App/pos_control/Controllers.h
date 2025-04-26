
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

  PIDParam()
  {
  }

  PIDParam(PIDParam& aPid)
  {
    Copy(aPid); on = 0;
  }

  void Copy(PIDParam& aPid)
  {
    KP = aPid.KP; KD = aPid.KD; KI = aPid.KI; _name = aPid._name;
    KS = aPid.KS; on = aPid.on;
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



