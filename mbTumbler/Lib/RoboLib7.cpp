
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
    GpIoInitInterrupt(_intrPin, encoder_isr, this, GPIO_INTR_POSEDGE);
  }
  else {
    GpIoInitInterrupt(_intrPin, encoder_isr2, this, GPIO_INTR_POSEDGE);
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



adc_oneshot_unit_handle_t  Adc2::_unit2 = 0;
adc_atten_t Adc2::atten = ADC_ATTEN_DB_11;

void Adc2::Init()
{
  if (_unit2 == 0) {
    adc_oneshot_unit_init_cfg_t inicfg = {
      .unit_id = ADC_UNIT_2,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&inicfg, &_unit2));
  }
  adc_oneshot_chan_cfg_t cfg2 = {
    .atten = Adc2::atten,
    .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(_unit2, _ch, &cfg2));
}

adc_oneshot_unit_handle_t  Adc1::_unit1 = 0;
adc_atten_t Adc1::atten = ADC_ATTEN_DB_11;

void Adc1::Init()
{
  if (_unit1 == 0) {
    adc_oneshot_unit_init_cfg_t inicfg = {
      .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&inicfg, &_unit1));
  }
  adc_oneshot_chan_cfg_t cfg2 = {
    .atten = Adc1::atten,
    .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(_unit1, _ch, &cfg2));
}



static void IRAM_ATTR dist_isr(void* arg)
{
  ((UsDist*)arg)->echoISR();
}

void UsDist::Init()
{
  GpIoInit(1ULL << trg, false);
  gpio_set_level(trg, 0);
  GpIoInitInterrupt(echo, dist_isr, this, GPIO_INTR_ANYEDGE);
}

void UsDist::startMeas()
{
  gpio_set_level(trg, 1);
  esp_rom_delay_us(12);
  gpio_set_level(trg, 0);
}

void UsDist::echoISR()
{
  if (gpio_get_level(echo))
    stw.Reset();
  else
    dist = stw.val();
}


