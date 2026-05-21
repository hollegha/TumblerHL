
#pragma once

class RateLim {
public:
  float F_SAMPLE;
public:
  float in, out;
public:
  RateLim(float aRate)
  {
    F_SAMPLE = 300.0;
    in = out = 0; setRateSec(aRate);
  }
  
  void reset()
  {
    in = out = 0;
  }
  
  // 1000V in 300 Ticks = 1000/300
  void setRateTicks(float aRate);

  // eg. 1000     = 1000V/Sec
  // eg. 1000/10  = 1000V/10Sec
  // e.g 1000/0.1 = 1000V/0.1Sec
  void setRateSec(float aRate)
  {
    _rate = aRate / F_SAMPLE;
  }

  // wird mit F_SAMPLE aufgerufen
  void calcOneStep();
public:
  float _rate;
};


