
#include "RoboLib7.h"
#include "EspMotor.h"

PwmTim RcServo::tim;
PwmTim Motor::tim;

void Motor::InitTimer()
{
  tim = InitPwmTimer(0, 1000000, 1000);
}

void Motor::StartTimer()
{
  ::StartPwmTimer(tim);
}

void Motor::Init()
{
  hpwm = InitPwm(tim, _pwm);
  GpIoInit(1ULL << _fwd, false);
  if (_rev != -1)
    GpIoInit(1ULL << _rev, false);
}


void GpIoInit(uint64_t aBitMask, bool aIn)
{
  gpio_config_t conf = {
    .pin_bit_mask = aBitMask,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
  };
  if (aIn) {
    conf.mode = GPIO_MODE_INPUT;
    conf.pull_up_en = GPIO_PULLUP_ENABLE;
  }
  ESP_ERROR_CHECK(gpio_config(&conf));
}



static void IRAM_ATTR encoder_isr(void* arg)
{
	((Encoder*)arg)->ISRFunction();
}

static void IRAM_ATTR encoder_isr2(void* arg)
{
  ((Encoder*)arg)->ISRFunction2();
}

inline int AbsDiff(uint32_t a, uint32_t b)
{
  int diff = a - b;
  if (diff < 0)
    return -diff;
  else
    return diff;
}

Encoder::Encoder(int aInt, int aDir, Motor* aMot)
{
  _intrPin = aInt; _dirPin = aDir; 
  inv = false;
}

void Encoder::Init()
{
  if (_dirPin != -1) {
    GpIoInit(1ULL << _dirPin, true);
    GpIoInitInterrupt(_intrPin, encoder_isr, this);
  }
  else {
    GpIoInitInterrupt(_intrPin, encoder_isr2, this);
  }
}

void Encoder::ISRFunction()
{
  checkDir();
}

void Encoder::checkDir()
{
  if (!inv) {
    if (gpio_get_level((gpio_num_t)_dirPin)) {
      cnt++; 
    }
    else {
      cnt--; 
    }
  }
  else {
    if (gpio_get_level((gpio_num_t)_dirPin)) {
      cnt--; 
    }
    else {
      cnt++; 
    }
  }
}

void Encoder::ISRFunction2()
{
  // checkDir();
}


