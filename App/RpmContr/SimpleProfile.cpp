
#include "RpmController.h"

#define CHECK_CANCEL() if( drCmd.doCancel() ) goto StopAll


void SimpleProfile(int dist, int vmax, int amax)
{
  ua0.SvPrintf("SP %d %d %d", dist, vmax, amax);
  encL.cnt=0; encR.cnt=0;
  rpmCL.setRate(amax); rpmCR.setRate(amax);
  rpmCL.setDemand(vmax); rpmCR.setDemand(vmax);
  rpmPara.on=1;
  while (encL.absCnt() < dist) {
    CHECK_CANCEL();
    vTaskDelay(1);
  }
  rpmCL.setDemand(0); rpmCR.setDemand(0);
  while (encL.getFrequF() > 50) {
    CHECK_CANCEL();
    vTaskDelay(1);
  }
StopAll:
  rpmCL.setDemand(0); rpmCR.setDemand(0);
}


void DriveTask(void* arg)
{
  int cmd;
  while (1) {
    cmd = drCmd.recv();
    if (cmd == 1) {
      SimpleProfile(ua0.ReadI16(), ua0.ReadI16(), ua0.ReadI16());
      ua0.SvPrintf("Prof finished!!");
    }
  }
}


extern "C" void app_main(void)
{
  printf("SimpleProfile_1\n");
  InitRtEnvHL();
  InitMotors(); InitRpmPID();
  InitUart(UART_NUM_0, 500000);
  // InitSoftAp("sepp", 5);
  EspTimSetup(300, RtTask, 0, false);
  xTaskCreate(DriveTask, "drt", 2048, NULL, 8, &(drCmd.t_recv));
  CommandLoop();
}

