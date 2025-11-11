
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
// #include "driver/gpio.h"
#include "MotorSetup.h"

SvProtocol3 ua0;

void JoyDrive(float aX, float aY);

void CommandLoop()
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 3) {
      encL.cnt = encR.cnt = 0;
      ua0.SvMessage("reset enc");
    }
    else if (cmd == 55) {
      JoyDrive(ua0.ReadI16(), ua0.ReadI16());
    }
    else if (cmd == 50) {
      esp_restart();
    }
  }
}

void JoyDrive(float aX, float aY)
{
  float powL = (aX + aY) / 1000.0;
  float powR = (aX - aY) / 1000.0;
  motL.setPow2(powL); motR.setPow2(powR);
}

extern "C" void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1);
    if (ua0.acqON) {
      ua0.WriteSvI16(1, (int)encL.getFrequ());
      ua0.WriteSvI16(2, (int)encR.getFrequ());
      ua0.Flush();
    }
  }
}

extern "C" void app_main(void)
{
  printf("JoyDrive\n");
  InitRtEnvHL(); 
  InitIO();
  // MyDelay(100); InitUart(UART_NUM_0, 500000); MyDelay(100);
  InitSoftAp("sepp", 1);
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL);
  CommandLoop();
}
