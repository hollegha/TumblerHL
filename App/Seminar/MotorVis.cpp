
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "driver/gpio.h"
#include "MotorSetup.h"

SvProtocol3 ua0;

void CommandLoop()
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) {
      float pow = ua0.ReadF();
      motR.setPow2(pow); motL.setPow2(pow);
      ua0.SvMessage("Set Pow");
    }
    if (cmd == 3) {
      encL.cnt = encR.cnt = 0;
      ua0.SvMessage("reset enc");
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
      ua0.WriteSvI16(1, (int)encL.cnt);
      ua0.WriteSvI16(2, (int)encR.cnt);
      ua0.Flush();
    }
  }
}

extern "C" void app_main(void)
{
  printf("MotorVis_2\n");
  InitRtEnvHL(); 
  InitIO();
  MyDelay(100); InitUart(UART_NUM_0, 500000); MyDelay(100); 
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL); 
  CommandLoop();
}
