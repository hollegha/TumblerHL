
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "MPU_Esp.h"
#include "ImuAlgo.h"
#include "FloatFilt.h"
#include "ComplFilt.h"

SvProtocol3 ua0;
extern MPU6050 mpu; // mpu rawData
bool calFlag = false;

ComplFilt cmpf;

void CommandLoop()
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) {
      ua0.SvMessage("cal start");
      calFlag = true; vTaskDelay(3);
      cmpf.calGyro(); cmpf.reset();
      calFlag = false;
      ua0.SvPrintf("calE %d", cmpf.gyOffs);
    }
    else if (cmd == 3) {
      ua0.SvMessage("reset integ");
      cmpf.reset();
    }
  }
}

void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1); // 100Hz
    if (ua0.acqON) {
      ua0.WriteSvF(1, cmpf.omega);
      ua0.WriteSvF(2, cmpf.gySum);
      ua0.WriteSvF(3, cmpf.accAngle);
      ua0.WriteSvF(4, cmpf.complAngle);
      // ua0.WriteSvF(4, cmpf.complAngle);
      ua0.Flush();
    }
  }
}

// x, y, z  0, 1, 2
void RglTask(void* arg)
{
  if (calFlag)
    return;
  mpu.getAccel(); // acc x,y,z holen
  mpu.getGyroX(); // I2C holen
  cmpf.calcFilt(mpu.acc[1], mpu.gyro[0]);
}

extern "C" void app_main(void)
{
  printf("GyroLab2_7\n");
  InitRtEnvHL(false); 
  initMPU();
  InitUart(UART_NUM_0, 500000);
  // InitSoftAp("sepp", 5);
  createTask(Monitor, "Monitor", 2048, 10);
  EspTimSetup(1000, RglTask, 0, false);
  CommandLoop();
}
