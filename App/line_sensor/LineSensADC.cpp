
#include "RTEnvHL.h"
#include "SvProtocol3.h"

SvProtocol3 ua0;

/* Adc1 ls1(ADC_CHANNEL_0);
Adc1 ls2(ADC_CHANNEL_3); 
Adc1 ls3(ADC_CHANNEL_6); 
Adc1 ls4(ADC_CHANNEL_7); */
Adc1 ls1(ADC_CHANNEL_0);
Adc1 ls2(ADC_CHANNEL_6);
Adc1 ls3(ADC_CHANNEL_7);
Adc1 ls4(ADC_CHANNEL_3);


void InitIO()
{
  Adc1::atten = ADC_ATTEN_DB_11;
  ls1.Init(); ls2.Init(); ls3.Init();
  ls4.Init(); 
}

void CommandLoop()
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 50) {
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
      ua0.Flush();
    }
  }
}

extern "C" void app_main(void)
{
	printf("LineSensTest_1\n");
  InitRtEnvHL();
  InitIO();
  InitUart(UART_NUM_0, 500000);
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL); // 10=Prio
  CommandLoop();
}











