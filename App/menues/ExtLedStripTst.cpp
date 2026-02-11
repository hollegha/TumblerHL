
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "LedStripHL.h"

SvProtocol3 ua0;

// slider = 3, 0:I, val:I

float ledAmpl = 0;

void CommandLoop(void* arg)
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 3) {
      ua0.ReadI16(); // idx
      ledAmpl = ua0.ReadI16();
      for(int i=0; i<=7; i++)
        leds.setAmplitude(i, ledAmpl / 1000.0);
      leds.refresh();
    }
  }
}

extern "C" void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1);
    if (ua0.acqON) {
      ua0.WriteSvI16(1, ledAmpl);
      ua0.Flush();
    }
  }
}

extern "C" void app_main(void)
{
  printf("ExtLedStrip test\n");
  InitRtEnvHL(); 
  InitUart(UART_NUM_0, 500000);
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL);
  CommandLoop(0);
}
