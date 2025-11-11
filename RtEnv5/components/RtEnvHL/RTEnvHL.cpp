
#include "RTEnvHL.h"
#include "EspMotor.h"
#include "SvProtocol3.h"
#include "FloatFilt.h"
#include "NodeLock.h"
#include "Features.h"

void LibVers()
{
  printf("RtEnvHL V2.1\n");
}

void InitRtEnvHL()
{
  LibVers();
  SvProtocol3::Init();
  OpenNVS();
  CheckNodeLock();
  CloseNVS();
}

void GpIoInit(uint64_t aBitMask, bool aIn)
{
  ChkFeature2(FEAT_SEM);
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












