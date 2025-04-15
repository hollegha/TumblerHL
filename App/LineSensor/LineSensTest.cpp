
#include "RTEnvHL.h"
#include "SvProtocol2.h"
#include "EspMotor.h"
#include "driver/gpio.h"

SvProtocol2 ua0;

GpIoOut irLed(23);
Adc1 ls1(ADC_CHANNEL_0); // GP36
Adc1 ls2(ADC_CHANNEL_3); // GP39
Adc1 ls3(ADC_CHANNEL_6); // GP34
Adc1 ls4(ADC_CHANNEL_7); // GP35
Adc1 ls5(ADC_CHANNEL_4);
Adc1 ls6(ADC_CHANNEL_5);

void InitIO()
{
  irLed.Init(); irLed.Set(0);
  Adc1::atten = ADC_ATTEN_DB_11;
  ls1.Init(); ls2.Init(); ls3.Init();
  ls4.Init(); ls5.Init(); ls6.Init();
}

void CommandLoop()
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) {
      int onOff = ua0.ReadI16(); irLed.Set(onOff);
      ua0.SvMessage2("IR", onOff);
    }
    else if (cmd == 50) {
      esp_restart();
    }
  }
}

extern "C" void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1);
    if (ua0.acqON) {
      ua0.WriteSvI16(1, ls1.read());
      ua0.WriteSvI16(2, ls2.read());
      ua0.WriteSvI16(3, ls3.read());
      ua0.WriteSvI16(4, ls4.read());
      ua0.WriteSvI16(5, ls5.read());
      ua0.WriteSvI16(6, ls6.read());
      ua0.Flush();
    }
  }
}

extern "C" void app_main(void)
{
	printf("LineSensTest_1\n");
  InitRtEnvHL();
  InitIO();
  MyDelay(100); InitUart(UART_NUM_0, 500000); MyDelay(100);
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL); // 10=Prio
  CommandLoop();
}











