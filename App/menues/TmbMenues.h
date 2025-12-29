
#pragma once

#include "RTEnvHL.h"
#include "LedStripHL.h"

// !!! ButtonHandler and LedStripHL !!!
//      are instantiated her
// !!! dont declare them elesewhere !!!

void InitTmbMenue();

// Menue points are switched by time
// 1-click selects
int execMenue();

// Menue points are switched by single click
// double-click selects
int execMenueDBL();

class ButtonHandler {
private:
  int _pin;
  SemaphoreHandle_t _sema;
  bool _waiting;
public:
  ButtonHandler(int aPin)
  {
    _pin = aPin; cnt = 0; _waiting = false;
  }
  void Init();
  void ISRFunction();
  void wait();
  int check()
  {
    return gpio_get_level((gpio_num_t)_pin);
  }
  void listen()
  {
    cnt = 0;
  }
  bool wasClicked()
  {
    return cnt != 0;
  }
public:
  int cnt;
};

extern ButtonHandler hbtn;
extern LedStripHL leds;

