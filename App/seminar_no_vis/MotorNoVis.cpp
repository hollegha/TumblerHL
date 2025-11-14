
#include "RTEnvHL.h"
#include "SvProtocol2.h"
#include "EspMotor.h"
#include "driver/gpio.h"
#include "MotorSetup.h"

// GpIoIn btn(17);

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
  while( (int)encR.cnt>-dist)
    vTaskDelay(1);
  motR.setPow2(0); motL.setPow2(0);
}

void PingPong()
{
  // wait until btn pressed
  // while( btn.get()!=1 )
    // MyDelay(10); // msec

  while (1) {
    Foreward(0.2, 200);
    MyDelay(300);
    Backward(0.2, 200);
    MyDelay(300);
  }
}

extern "C" void app_main(void)
{
  printf("MotorNoVis\n");
  InitRtEnvHL(); // btn.Init();
  InitIO();
  PingPong();
}
