
#pragma once

#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_timer.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/dac_oneshot.h"
#include "TimerHL.h"

void LibVers();
void InitRtEnvHL();

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


class Adc2 {
private:
  adc_channel_t _ch;
  static adc_oneshot_unit_handle_t _unit2;
public:
  static adc_atten_t atten;
public:
  Adc2(adc_channel_t aChan)
  {
    _ch = aChan;
  }
  void Init();
  int read()
  {
    int val;
    adc_oneshot_read(_unit2, _ch, &val);
    return val;
  }
};

class Adc1 {
private:
  adc_channel_t _ch;
  static adc_oneshot_unit_handle_t _unit1;
public:
  static adc_atten_t atten;
public:
  Adc1(adc_channel_t aChan)
  {
    _ch = aChan;
  }
  void Init();
  int read()
  {
    int val;
    adc_oneshot_read(_unit1, _ch, &val);
    return val;
  }
};


class Dac {
public:
  Dac(dac_channel_t aChan)
  {
    _ch = aChan;
  }
  void Init()
  {
    dac_oneshot_config_t cfg = {
    .chan_id = _ch,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&cfg, &_hdac));
  }
  void write(int aVal)
  {
    dac_oneshot_output_voltage(_hdac, aVal);
  }
  void writeF(float aVal)
  {
    uint8_t dval = (aVal * 0.5 + 0.5) * 256.0;
    dac_oneshot_output_voltage(_hdac, dval);
  }
private:
  dac_channel_t _ch;
  dac_oneshot_handle_t _hdac;
};


class UsDist {
  gpio_num_t trg, echo;
  StopWatch stw;
public:
  UsDist(int aTrg, int aEcho)
  {
    trg = (gpio_num_t)aTrg;
    echo = (gpio_num_t)aEcho;
    dist = 0;
  }
  void Init();
  void startMeas();
  void echoISR();
public:
  uint32_t dist;
};



