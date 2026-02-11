
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


void RotStep(float pow)
{
  motL.setPow2(-pow); motR.setPow2(pow);
  vTaskDelay(5);
  motL.setPow2(0); motR.setPow2(0);
  MyDelay(200);
}

// RawVals !! this is calibration !!
void Turn2EndOfSensor(float pow)
{
  // ua0.SvPrintf("TTE %1.2f", pow);
  int idx = 5;
  if (pow > 0) idx = 0;
  while( ls.rawVal(idx) < 1000 ) {
    RotStep(pow);
    CHECK_CANCEL();
  }
  MyDelay(300);
  encL.cnt = encR.cnt = 0;
  motL.setPow2(-pow); motR.setPow2(pow);
  while (encL.absCnt() < 40) {
    CHECK_CANCEL();
    vTaskDelay(1);
  }
StopAll:
  motL.setPow2(0); motR.setPow2(0);
}

void AutoCal(float pow)
{
  lpd.onOff(0);
  while (1) {
    Turn2EndOfSensor(-pow);
    CHECK_CANCEL2();
    MyDelay(1000);
    Turn2EndOfSensor(pow);
    CHECK_CANCEL2();
    MyDelay(1000);
  }
}


bool CkeckEOL()
{
  if (ls.allZero()) {
    MyDelay(100);
    if (ls.allZero())
      return true;
  }
  return false;
}

void FollowLine(float pow, bool stopAtEnd)
{
  ls.mode = CAL_VALS; lpd.forew = pow;
  lpd.onOff(1);
  while (1) {
    vTaskDelay(1);
    CHECK_CANCEL();
    if (!ls.floorVisible())
      break;
    if (stopAtEnd && CkeckEOL())
      break;
  }
StopAll:
  lpd.onOff(0); vTaskDelay(3);
  motL.setPow2(0); motR.setPow2(0);
}

void TurnUntilLine(float pow)
{
  lpd.onOff(0);
  motL.setPow2(-pow); motR.setPow2(pow);
  while (ls.Y(2) < 1000 && ls.Y(3) < 1000) {
    vTaskDelay(1);
    CHECK_CANCEL();
  }
StopAll:
  motL.setPow2(0); motR.setPow2(0);
}


void remoteControl()
{
  drCmd.t_recv = xTaskGetCurrentTaskHandle();
  int cmd;
  while (1) {
    cmd = drCmd.recv();
    if (cmd == 1) {
      float pow=ua0.ReadF();
      ua0.SvMessage("DrFL");
      FollowLine(pow,false); ua0.SvMessage("End Drv");
    }
    else if (cmd == 2) {
      float pow=ua0.ReadF(); 
      ua0.SvMessage("AutoCal"); 
      AutoCal(pow); ua0.SvMessage("End AC");
    }
    else if (cmd == 3) {
      float pow = ua0.ReadF();
      ua0.SvMessage("turn line");
      TurnUntilLine(pow); ua0.SvMessage("end turn");
    }
    else if (drCmd.doCancel())
      drCmd.clear();
  }
}


float selPow()
{
  int sel = execMenueDBL2(4);
  switch (sel) {
    case 1: return 0.2;
    case 2: return 0.4;
    case 3: return 0.6;
    case 4: return 0.8;
    default: return 0.2;
  }
}

void MenueDriveControl()
{
  int sel = 0;
  float pow = 0.2;
  while (1) {
    pow = selPow();
    sel = execMenueDBL();
    // ua0.SvPrintf("sel %d working...", sel);
    if (sel == 1)
      AutoCal(pow);
    if (sel == 2)
      FollowLine(pow,false);
    if (sel == 3) {
      while (1) {
        FollowLine(pow, true);
        if (!ls.floorVisible())
          break;
        MyDelay(500);
        TurnUntilLine(pow);
        MyDelay(500);
      }
    }
  }
}

extern "C" void app_main(void)
{
  printf("LineDemo1_3\n");
  InitRtEnvHL();
  InitMotors(); 
  InitUart(UART_NUM_0, 500000); 
  // InitSoftAp("franz_1", 1);
  InitLineControler();
  InitTmbMenue();
  // print_task_list();
  // remoteControl();
  MenueDriveControl();
}











