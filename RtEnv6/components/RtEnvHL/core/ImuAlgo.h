
#pragma once

#include "math.h"

class Tp1Ord;
class Tp2OrdF;

class ImuChan {
public:
  // type 1..acc, 2..gyro
  ImuChan(int* aValAry, int type);
  void CalcFilt();
  void CalcFilt(int idx);
  int getVal(int idx);

  // narrow BW 1Ord
  float getFilt1(int idx); // Tp1Ord
  // broad BW 2nd Ord
  float getFilt2(int idx); // Tp2Ord
  void SetAlpha(float aAlpha); // Tp1Ord

  void CalGyro(bool readSensor=true);
private:
  Tp1Ord* f1[3];
  Tp2OrdF* f2[3];
  int* ary;
  int offset[3];
};


class KalmSimple {
public:
  float accAngle;
  float omega;
  float complAngle; // kalman filtert
  float gySum;
public:
  KalmSimple() 
  {
    Reset();
  }
  void Reset()
  {
    accAngle = omega = complAngle = gySum = 0;
  }
  float getAbsAngle()
  {
    return fabs(complAngle);
  }
  void CalcFilter(float accVal, float gyroVal);
};
