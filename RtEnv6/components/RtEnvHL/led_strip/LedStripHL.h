
#pragma once
#include "led_strip.h"

const int H_RED = 5;
const int H_ORANGE = 18;
const int H_YELLOW1 = 30;
const int H_YELLOW2 = 40;
const int H_YELLOW3 = 50;
const int H_YEL_GREEN = 60;
const int H_GREEN1 = 70;
const int H_GREEN2 = 100;
const int H_GREEN_BLUE = 150;
const int H_BLUE = 200;
const int H_VIOLET = 355;

const uint8_t DEF_SAT = 243;
const uint8_t DEF_VAL = 40;

class LedStripHL {
public:
  uint8_t sat;
  uint8_t val;
private:
  uint8_t _toggleST;
public:
  LedStripHL()
  {
    sat = DEF_SAT; val = DEF_VAL;
    _toggleST = 0;
  }

  void Init();

  void setHue(int i, uint16_t aHue)
  {
    led_strip_set_pixel_hsv(_ls, i, aHue, sat, val);
  }

  void ledOFF(int from, int to)
  {
    val = 0;
    for (int i = from; i <= to; i++)
      setHue(i, H_VIOLET);
    val = DEF_VAL;
  }

  void setHueFT(int from, int to, uint16_t aHue)
  {
    for (int i = from; i <= to; i++)
      setHue(i, aHue);
  }

  void toggle(int from, int to, uint16_t aHue)
  {
    if (_toggleST) {
      ledOFF(from, to);
      _toggleST = 0;
    }
    else {
      setHueFT(from, to, aHue);
      _toggleST = 1;
    }
  }
  
  void blink(int from, int to, uint16_t aHue, int aN);

  void setRGB(int i, int r, int g, int b)
  {
    led_strip_set_pixel(_ls, i, r, g, b);
  }

  // amplitude indicator
  void setAmplitude(int i, float val);
  
  void refresh()
  {
    led_strip_refresh(_ls);
  }

  void clear()
  {
    led_strip_clear(_ls);
  }

private:
  led_strip_handle_t _ls;
};

extern LedStripHL leds;