
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "driver/gpio.h"
#include "MotorSetup.h"

SvProtocol3 ua0;
int remCommand = 0;

void CommandLoop()
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 3) {
      encL.cnt = encR.cnt = 0;
      ua0.SvMessage("reset enc");
    }
    if (cmd == 4) {
      remCommand = ua0.ReadI16();
      ua0.SvMessage("rem cmd");
    }
    else if (cmd == 50) {
      esp_restart();
    }
  }
}


void Foreward(float pow, int dist)
{
  encR.cnt = encL.cnt = 0;
  motR.setPow2(pow); motL.setPow2(pow);
  while (encR.cnt < dist)
    vTaskDelay(1);
  motR.setPow2(0); motL.setPow2(0);
}

void Backward(float pow, int dist)
{
  encR.cnt = encL.cnt = 0;
  motR.setPow2(-pow); motL.setPow2(-pow);
  while ((int)encR.cnt > -dist)
    vTaskDelay(1);
  motR.setPow2(0); motL.setPow2(0);
}

extern "C" void DriveTask(void* arg)
{
  while (1) {
    while (remCommand == 0)
      MyDelay(100); // 100msec
    
    if (remCommand == 1) {
      remCommand = 0;
      Foreward(0.2, 200);
    }
    
    if (remCommand == 2) {
      remCommand = 0;
      Backward(0.2, 200);
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
  printf("Rt5 Remote commands\n");
  InitRtEnvHL(); 
  InitIO();
  // MyDelay(100); InitUart(UART_NUM_0, 500000); MyDelay(100);
  InitSoftAp("sepp", 1);
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL);
  xTaskCreate(DriveTask, "Drv", 2048, NULL, 10, NULL);
  CommandLoop();
}
