
#include <stdio.h>
#include <inttypes.h>
#include "RTEnvHL.h"
#include "SvProtocol2.h"
#include "EspMotor.h"
#include "MPU_Esp.h"
#include "driver/gpio.h"

SvProtocol2 ua0;
MPU6050 mpu;
int dispMode = 1;

// ay Gx

void CommandLoop()
{
  // printf("CommandLoop\n");
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) {
      dispMode = ua0.ReadI16();
      if (dispMode == 1)
        ua0.SvMessage("Disp Acc");
      if (dispMode == 2)
        ua0.SvMessage("Disp Gyro");
    }
    else if (cmd == 50) {
      esp_restart();
    }
  }
}

void DispAcc()
{
  mpu.getAccel();
  ua0.WriteSvI16(1, mpu.acc[0]);
  ua0.WriteSvI16(2, mpu.acc[1]);
  ua0.WriteSvI16(3, mpu.acc[2]);
}

void DispGyro()
{
  mpu.getGyro();
  ua0.WriteSvI16(1, mpu.gyro[0]);
  ua0.WriteSvI16(2, mpu.gyro[1]);
  ua0.WriteSvI16(3, mpu.gyro[2]);
}

extern "C" void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1);
    if (ua0.acqON) {
      if (dispMode == 1)
        DispAcc();
      if (dispMode == 2)
        DispGyro();
      ua0.Flush();
    }
  }
}

extern "C" void app_main(void)
{
  printf("MpuTest1_1\n");
  InitRtEnvHL(); printf("after InitRT\n");
  I2cInit(); printf("Conn: %X\n", mpu.testConnection()); mpu.Init();
  MyDelay(100); InitUart(0,500000); MyDelay(100);
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL);
  CommandLoop();
}
