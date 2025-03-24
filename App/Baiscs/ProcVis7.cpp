
#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "RTEnvHL.h"
#include "SvProtocol2.h"
#include "EspMotor.h"
#include "driver/gpio.h"

SvProtocol2 ua0;

void CommandLoop()
{
  printf("CommandLoop\n");
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) {
      int v1 = ua0.ReadI16(); int v2 = ua0.ReadI16();
      ua0.SvPrintf("Cmd2 %d %d", v1, v2);
    }
    else if (cmd == 50) {
      esp_restart();
    }
  }
}

extern "C" void Monitor(void* arg)
{
  int v1 = 0, v2 = 10; float v3 = 20;
  while(1) {
    vTaskDelay(10);
    if (ua0.acqON) {
      ua0.WriteSvI16(1, v1++);
      ua0.WriteSvI16(2, v2++);
      ua0.WriteSvF(3, v3 += 0.3);
      ua0.Flush();
    }
  }
}

extern "C" void app_main(void)
{
  printf("ProcVis7 RtEnv3\n");
  InitRtEnvHL(); printf("after InitRT\n");
  MyDelay(100); InitUart(UART_NUM_0, 500000); MyDelay(100);
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL);
  CommandLoop();
}
