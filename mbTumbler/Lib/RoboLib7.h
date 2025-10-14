
#pragma once

#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_timer.h"
#include "TimerHL.h"


inline void MyDelay(int aMSec)
{
  vTaskDelay(aMSec/portTICK_PERIOD_MS);
}

void GpIoInit(uint64_t aBitMask, bool aIn=false);

inline void GpIoDir(gpio_num_t aNum, gpio_mode_t aMode)
{
  gpio_set_direction(aNum, aMode);
}

inline void GpIoLevel(gpio_num_t aNum, uint32_t aVal)
{
  gpio_set_level(aNum, aVal);
}

class GpIoOut {
public:
  uint16_t pin;
  uint16_t val;
public:
  GpIoOut(int aPin)
  {
    pin = aPin; val = 0;
  }

  void Init()
  {
    GpIoInit(1ULL<<pin, false);
  }

  void Set(int aVal)
  {
    val = aVal; gpio_set_level((gpio_num_t)pin,val);
  }

  void Toggle()
  {
    if (val) Set(0);
    else Set(1);
  }
};


class GpIoIn {
public:
  uint32_t pin;
public:
  GpIoIn(int aPin)
  {
    pin = aPin;
  }
  void Init()
  {
    GpIoInit(1ULL<<pin, true);
  }
  int get()
  {
    return gpio_get_level((gpio_num_t)pin);
  }
};


class StopWatch {
public:
  int64_t dt;
public:
  StopWatch()
  {
    _t1 = dt = 0; 
  }
  void Reset()
  {
    _t1 = esp_timer_get_time();
  }
  int64_t val()
  {
    dt = esp_timer_get_time() - _t1;
    return dt;
  }
private:
  int64_t _t1;
};






