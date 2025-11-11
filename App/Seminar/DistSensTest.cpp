
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"

SvProtocol3 ua0;

UsDist ds(14, 27);

void CommandLoop()
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
  }
}

extern "C" void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(2);
    if (ua0.acqON) {
      ua0.WriteSvI16(1, ds.dist);
      ua0.Flush();
      ds.startMeas();
    }
  }
}

extern "C" void app_main(void)
{
  printf("DistTest\n");
  InitRtEnvHL(); 
  ds.Init();
  InitUart(UART_NUM_0, 500000);
  // InitSoftAp("sepp", 1);
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL);
  CommandLoop();
}
