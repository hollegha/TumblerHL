#pragma once

#include "Tp1Ord.h"

const float CAL_AMPL = 2000.0;

const int DISP_RAW =  1;
const int CALIBRATE = 2;
const int CAL_VALS  = 3;
const int POS_VALS = 4;
const int ENCODERS = 5;

class LineChannel {
  const float ALPHA = 0.07; // 0.07
  const float BETHA = 1 - ALPHA;
public:
  LineChannel()
  {
    initCal(); y = rawVal = cnt = 0;
  }
  void readADC()
  {
    rawVal = adc.read();
  }
  void initCal()
  {
    _min = 32000; _max = 0; y = 400;
  }
  void cal2_kd()
  {
    _k = CAL_AMPL / (float)(_max - _min);
  }
  void calcCalibration()
  {
    // calcFilt(idx);
    // y must be filtered
    if (((int)y) < _min)
      _min = y;
    if (((int)y) > _max)
      _max = y;
    if ((_max - _min) == 0)
      return;
    _k = CAL_AMPL / (float)(_max - _min);
  }
  void calcCalFilt() 
  {
    y = y * BETHA + (float)rawVal * ALPHA;
  }
  float calVal()
  {
    y = (rawVal - _min) * _k;
    if (y < 0) y = 0;
    return y;
  }
public:
  int rawVal;
  float y;
  int cnt; // lenght of crossing
public:
  int _min, _max;
  float _k;
public:
  Adc1 adc;
};


class LineSensor {
public:
  int mode;
  int _pos;
  float _posDiff;
protected:
  int N_LS_CHAN;
  LineChannel lsAry[6];
public:
  LineSensor()
  {
    N_LS_CHAN = 6;
    mode = CAL_VALS;
    _pos = z1 = z2 = 0;
    _posDiff = 0;
    tp.SetAlpha(0.5);
  }

  float Y(int i)
  {
    return lsAry[i].y;
  }
  int rawVal(int i)
  {
    return lsAry[i].rawVal;
  }

  void initCrossCnt()
  {
    for (int i = 0; i < N_LS_CHAN; i++)
      lsAry[i].cnt = 0;
  }
  void countCrossing(float aThrs)
  {
    for (int i = 0; i < N_LS_CHAN; i++) {
      if (lsAry[i].y > aThrs)
        lsAry[i].cnt++;
    }
  }
  int cossCnt(int i)
  {
    return lsAry[i].cnt;
  }
  
  void readADC()
  {
    for (int i = 0; i < N_LS_CHAN; i++)
      lsAry[i].readADC();
  }

  void dispOnLeds();
  void dispRawVals(SvProtocol3* ua)
  {
    for (int i = 0; i < N_LS_CHAN; i++)
      ua->WriteSvI16(i + 1, lsAry[i].rawVal);
  }
  void dispYVals(SvProtocol3* ua)
  {
    for (int i = 0; i < N_LS_CHAN; i++)
      ua->WriteSvI16(i + 1, lsAry[i].y);
  }
  void dispMinMax(SvProtocol3* ua)
  {
    ua->SvMessage("Min Max");
    for (int i = 0; i < N_LS_CHAN; i++)
      ua->SvPrintf("%d %d", lsAry[i]._min, lsAry[i]._max);
  }
  
  void setCal(int idx, int aMin, int aMax)
  {
    lsAry[idx]._min = aMin; lsAry[idx]._max = aMax;
    lsAry[idx]._k = CAL_AMPL / (float)(aMax - aMin);
  }
  void initCal()
  {
    for (int i = 0; i < N_LS_CHAN; i++)
      lsAry[i].initCal();
  }
  void calStep()
  { // y must be filtered
    for (int i = 0; i < N_LS_CHAN; i++)
      lsAry[i].calcCalibration();
  }
  void calcCalFilt()
  {
    for (int i = 0; i < N_LS_CHAN; i++)
      lsAry[i].calcCalFilt();
  }
  void calcCal()
  {
    for (int i = 0; i < N_LS_CHAN; i++)
      lsAry[i].calVal();
  }

  void cal2nvs();
  int nvs2cal();
public:
  Tp1Ord tp;
protected:
  int z1, z2;
};


class LsPololu : public LineSensor {
private:
  int _rgPos;
public:
  LsPololu() : LineSensor()
  {
    N_LS_CHAN = 6; _rgPos = 0;
  }

  float pos()
  {
    return (float)_pos * (1.0 / 3700.0); // 8000
  }
  float posDiff()
  {
    return _posDiff * (1.0 / 200.0);
  }

  void initADC();
  void setDefaultCalib();

  void calcPos();
  void calcPos2();

  bool checkRange(int idxL, int idxR);
  
  bool isMidZero()
  {
    const float LOW_LEVEL = 50;
    if (lsAry[1].y < LOW_LEVEL && lsAry[2].y < LOW_LEVEL
      && lsAry[3].y < LOW_LEVEL && lsAry[4].y < LOW_LEVEL)
      return true;
    return false;
  }
  bool allZero()
  {
    const float LOW_LEVEL = 60;
    for (int i = 0; i < N_LS_CHAN; i++)
      if (lsAry[i].y > LOW_LEVEL)
        return false;
    return true;
  }
  // uses rawVal
  bool floorVisible()
  {
    for (int i = 0; i < N_LS_CHAN; i++)
      if (lsAry[i].rawVal < 1000)
        return true;
    return false;
  }
  bool posInRange()
  {
    if (_pos<3700 && _pos>-3700) // 6500
      return true;
    return false;
  }
};


class LsBertl : public LineSensor {
public:
  LsBertl() : LineSensor()
  {
    N_LS_CHAN = 4;
  }

  void initADC();

  void setDefaultCalib();

  void experPos(); // experimental Position
};


