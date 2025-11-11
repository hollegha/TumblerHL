
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


inline float ClipHL(float aVal, float aMax)
{
  if (aVal > aMax)
    return aMax;
  if (aVal < -aMax)
    return -aMax;
  return aVal;
}


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



