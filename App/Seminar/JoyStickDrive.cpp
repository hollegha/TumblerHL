
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
// #include "driver/gpio.h"
#include "MotorSetup.h"

SvProtocol3 ua0;

int joyX, joyY;

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
      joyX = ua0.ReadI16(); joyY = ua0.ReadI16();
      // JoyDrive(joyX, joyX);
    }
    else if (cmd == 50) {
      esp_restart();
    }
  }
}

void JoyDrive(float aX, float aY)
{
  float forew = aX / 1000.0;
  float curve = (aY * 0.5) / 1000.0;
  float powL = (forew + curve);
  float powR = (forew - curve);
  motL.setPow2(powL); motR.setPow2(powR);
}

extern "C" void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1);
    if (ua0.acqON) {
      ua0.WriteSvI16(1, joyX);
      ua0.WriteSvI16(2, joyY);
      ua0.Flush();
    }
  }
}

extern "C" void app_main(void)
{
  printf("JoyDrive\n");
  InitRtEnvHL(); 
  InitIO();
  // InitUart(UART_NUM_0, 500000); 
  InitSoftAp("sepp", 1);
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL);
  CommandLoop();
}
