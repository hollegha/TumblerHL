
#include <stdio.h>
#include "RTEnvHL.h"
#include "SvProtocol2.h"
#include "MPU_Esp.h"
#include "ImuAlgo.h"

SvProtocol2 ua0;
MPU6050 mpu;
ImuChan imuAcc(mpu.acc, 1);
ImuChan imuGyro(mpu.gyro, 2);
int dispMode=1; 
StopWatch stw;
int chanIdx = 0;
bool calFlag = false;

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
    if (cmd == 3) {
      imuAcc.SetAlpha(ua0.ReadF());
      ua0.SvMessage("Set Alpha");
    }
    if (cmd == 4) {
      ua0.SvPrintf("dt %d", (uint32_t)stw.dt);
    }
    if (cmd == 5) {
      chanIdx = ua0.ReadI16();
      if (chanIdx > 2) chanIdx = 2;
      ua0.SvPrintf("ChanIdx %d", chanIdx);
    }
    if (cmd == 6) {
      calFlag=true; ua0.SvMessage("Cal start");
      imuGyro.CalGyro();
      calFlag=false; ua0.SvMessage("Cal done!!");
    }
    else if (cmd == 50) {
      esp_restart();
    }
  }
}


void DoDisplay()
{
  ImuChan* di = &imuAcc;
  if (ua0.acqON) {
    if (dispMode == 1) di = &imuAcc;
    else di = &imuGyro;
    ua0.WriteSvI16(1, di->getVal(0));
    ua0.WriteSvI16(2, di->getFilt2(0));
    // ua0.WriteSvI16(3, di->getFilt2(2));
    ua0.Flush();
  }
}

extern "C" void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1);
    DoDisplay();
  }
}

extern "C" void RglTask(void* arg)
{
  stw.Reset();
  if (calFlag) 
    return;
  mpu.getAccel(); mpu.getGyro();
  imuAcc.CalcFilt(); imuGyro.CalcFilt();
  stw.val();
}

// accY gyroX

extern "C" void app_main(void)
{
  printf("ImuTest2_3\n");
  InitRtEnvHL(); printf("after InitRT\n");
  I2cInit(); printf("Conn: %X\n", mpu.testConnection()); mpu.Init();
  MyDelay(100); InitUart(0, 500000); MyDelay(100);
  // configMAX_PRIORITIES = 25
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 1, NULL);
  EspTimSetup(1000, RglTask, false);
  CommandLoop();
}
