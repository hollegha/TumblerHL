
#pragma once

#include "TimerHL.h"
#include "math.h"

class Tp2OrdF;

class Encoder2 {
  const float FREQU_FACT = 1E6;
  const float DT_FACT = 150.0;
public:
  Encoder2(int aInt, int aDir);
  void Init();

  void ISRFunction();
  
  void setBW(int aFiltNum); // 1..small -> 5..big 0..off
  
  void CalcFilt2();

  void CalcDiff(float aVal);
  // unsigned
  float getFrequUS() 
  {
    return (1.0 / (float)pw) * FREQU_FACT;
  }
  // signed
  float getFrequ() 
  {
    if (actDir > 0)
      return (1.0 / (float)pw) * FREQU_FACT;
    else if (actDir < 0)
      return (-1.0 / (float)pw) * FREQU_FACT;
    else
      return 0.0;
  }
  // filtert
  float getFrequF(); 
  float getDiff()
  {
    return diff * DT_FACT;
  }
  int getPW()
  {
    if (pw > 32000)
      return 32000;
    else
      return pw;
  }
  int getDir()
  {
    return actDir;
  }

  void checkDir();

  int absCnt()
  {
    if ((int)cnt < 0)
      return -(int)cnt;
    return cnt;
  }
private:
  int _dirPin, _intrPin;
  float actDir;
  bool filtOn;
  int64_t _t1;
  uint32_t _runFlag;
  Tp2OrdF* _filt;
  float z1, z2, z3;
public:
  int cnt;
  uint32_t pw;
  float diff;
  bool inv;
  int info;
};






