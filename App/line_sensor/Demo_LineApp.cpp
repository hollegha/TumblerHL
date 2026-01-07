
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
  int idx = 0;
  if (pow > 0) idx = 5;
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
    MyDelay(500);
    if (ls.allZero())
      return true;
  }
  return false;
  /* if (ls.allZero()) {
    for (int i = 1; i <= 5; i++) {
      vTaskDelay(1);
      if( !ls.allZero() )
        return false;
    }
    return true;
  }
  return false; */
}

void DriveUntilLost(float pow)
{
  lpd.forew = pow;
  lpd.onOff(1);
  while (1) {
    vTaskDelay(1);
    CHECK_CANCEL();
    if( CkeckEOL() )
      break;
    if (!ls.floorVisible())
      break;
  }
StopAll:
  lpd.onOff(0);
  vTaskDelay(3);
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


extern "C" void DriveControl(void* arg)
{
  int cmd;
  while (1) {
    cmd = drCmd.recv();
    if (cmd == 1) {
      float pow=ua0.ReadF();
      ua0.SvMessage("DrFL");
      DriveUntilLost(pow); ua0.SvMessage("End Drv");
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


void MenueDriveControl()
{
  int sel = 0;
  while (1) {
    sel = execMenueDBL();
    // ua0.SvPrintf("sel %d working...", sel);
    if (sel == 1)
      AutoCal(0.1);
    if (sel == 2)
      DriveUntilLost(0.2);
  }
}


extern "C" void app_main(void)
{
	printf("LineDemo1_3\n");
  InitRtEnvHL();
  InitMotors(); 
  // InitUart(UART_NUM_0, 500000);
  InitSoftAp("sepp", 5);
  InitLineControler();
  // InitTmbMenue();
  // xTaskCreate(DriveControl, "Drc", 2048, NULL, 8, &(drCmd.t_recv));
  // MenueDriveControl();
  while (1)
    MyDelay(10000);
}











