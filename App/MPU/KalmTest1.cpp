
#include <stdio.h>
#include "RTEnvHL.h"
#include "SvProtocol2.h"
#include "MPU_Esp.h"
#include "ImuAlgo.h"

SvProtocol2 ua0;
MPU6050 mpu;
ImuChan imuAcc(mpu.acc, 1);
ImuChan imuGyro(mpu.gyro, 2);
KalmSimple kalm;
int dispMode=3; 
StopWatch stw;
bool calFlag = false;

void DoDisplay();
void SetDispMode();

void CommandLoop()
{
  // printf("CommandLoop\n");
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) {
      SetDispMode();
    }
    if (cmd == 3) {
      imuAcc.SetAlpha(ua0.ReadF());
      ua0.SvMessage("Set Alpha");
    }
    if (cmd == 4) {
      ua0.SvPrintf("dt %d", (uint32_t)stw.dt);
    }
    if (cmd == 5) {
      kalm.Reset();
      ua0.SvMessage("Reset Kalm");
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

extern "C" void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1);
    DoDisplay();
  }
}

// accY gyroX x,y,z  0,1,2
extern "C" void RglTask(void* arg)
{
  stw.Reset();
  if (calFlag) 
    return;
  mpu.getAccelY(); mpu.getGyroX();
  imuAcc.CalcFilt(1); imuGyro.CalcFilt(0);
  kalm.CalcFilter(imuAcc.getFilt2(1), imuGyro.getFilt2(0));
  stw.val();
}

void DoDisplay()
{
  if (ua0.acqON) {
    if (dispMode == 1) { // acc
      ua0.WriteSvI16(1, imuAcc.getVal(1));
      ua0.WriteSvI16(2, imuAcc.getFilt2(1));
    }
    if (dispMode == 2) { // gyro
      ua0.WriteSvI16(1, imuGyro.getVal(0));
      ua0.WriteSvI16(2, imuGyro.getFilt2(0));
    }
    if (dispMode == 3) { // KalmSimple
      ua0.WriteSvI16(1, kalm.accAngle);
      ua0.WriteSvI16(2, kalm.gySum);
      ua0.WriteSvI16(3, kalm.complAngle);
    }
    ua0.Flush();
  }
}

void SetDispMode()
{
  dispMode = ua0.ReadI16();
  if (dispMode == 1)
    ua0.SvMessage("Disp Acc");
  if (dispMode == 2)
    ua0.SvMessage("Disp Gyro");
  if (dispMode == 3)
    ua0.SvMessage("Disp KalmSimple");
}


extern "C" void app_main(void)
{
  printf("KalmTest1_3\n");
  InitRtEnvHL(); 
  I2cInit(); printf("Conn: %X\n", mpu.testConnection()); mpu.Init();
  MyDelay(100); InitUart(UART_NUM_0, 500000); MyDelay(100);
  // configMAX_PRIORITIES = 25
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 1, NULL);
  EspTimSetup(1000, RglTask, false);
  CommandLoop();
}
