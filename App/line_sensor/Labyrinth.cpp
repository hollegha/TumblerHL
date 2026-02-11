
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "LineController.h"
#include "TmbMenues.h"
#include "MotorSetup.h"

extern LsPololu ls;
extern LinePID lpd;
SvProtocol3 ua0;

#define CHECK_CANCEL() if( hbtn.check() || drCmd.doCancel() ) goto StopAll
#define CHECK_CANCEL2() if( hbtn.check() || drCmd.doCancel() ) return

// Thrsh=1000 for counting

// 92

bool CkeckEOL()
{
  if (ls.allZero()) {
    MyDelay(100);
    if (ls.allZero())
      return true;
  }
  return false;
}

bool isCrossing()
{
  const float LIM = 1000.0;
  if (ls.Y(0) > LIM || ls.Y(5) > LIM)
    return true;
  if (ls.Y(1) > LIM || ls.Y(4) > LIM)
    return true;
  return false;
}

void driveUntilCrossing(float pow)
{
  ls.mode = CAL_VALS; lpd.forew = pow;
  lpd.onOff(1);
  while (1) {
    vTaskDelay(1);
    if (CkeckEOL())
      break;
    if (isCrossing()) {
      lpd.onOff(0);
      motL.setPow2(pow); motR.setPow2(pow);
      ls.initCrossCnt();
      while (isCrossing()) {
        ls.countCrossing(1000.0);
        vTaskDelay(1);
      }
      break;
    }
  }
  encL.cnt = 0;
  while( encL.cnt<75 )
    vTaskDelay(1);
  motL.setPow2(0); motR.setPow2(0);
  ua0.SvPrintf("cross %d  %d", ls.cossCnt(0), ls.cossCnt(5));
}


void drive2end(float pow)
{
  ls.mode = CAL_VALS; lpd.forew = pow;
  lpd.onOff(1);
  while (1) {
    vTaskDelay(1);
    /* if (isCrossing())
      lpd.onOff(0);
    else
      lpd.onOff(1); */
    if (CkeckEOL())
      break;
  }
  lpd.onOff(0); vTaskDelay(1);
  motL.setPow2(0); motR.setPow2(0);
}

void remoteControl()
{
  drCmd.t_recv = xTaskGetCurrentTaskHandle();
  int cmd;
  while (1) {
    cmd = drCmd.recv();
    if (cmd == 5) {
      float pow = ua0.ReadF();
      ua0.SvMessage("DrCross");
      driveUntilCrossing(pow); ua0.SvMessage("End Drv");
    }
    if (cmd == 6) {
      float pow = ua0.ReadF();
      ua0.SvMessage("DrEnd");
      drive2end(pow); ua0.SvMessage("End Drv");
    }
    else if (drCmd.doCancel())
      drCmd.clear();
  }
}

extern "C" void app_main(void)
{
  printf("Labyrinth_1\n");
  InitRtEnvHL();
  InitMotors(); 
  // InitUart(UART_NUM_0, 500000);
  InitSoftAp("sepp", 5);
  InitLineControler();
  remoteControl();
}











