
#include <stdio.h>
#include "RTEnvHL.h"
#include "SvProtocol2.h"
#include "EspMotor.h"
#include "MPU_Esp.h"
#include "ImuAlgo.h"
#include "BalanceControl.h"
#include "Balance_IO.h"

SvProtocol2 ua0;

float angleOffs = 10.0;

// KP KD KI KS
BalanceController balC(2E-2, 4E-4, 0, 3E-3, "BalC");

void DoDisplay();
void SetDispMode();

void ControlParams(int cmd)
{
  if (cmd == 200) {
    int num = ua0.ReadI16();
    balC.ReadCOM(&ua0);
    ua0.SvMessage("Set PID");
    // balC.Print(&prot0);
  }
  if (cmd == 201) {
    int num = ua0.ReadI16();
    LockOStream();
    ua0.WrB(200);
    balC.WriteCOM(&ua0);
    UnlockOStream();
    ua0.SvMessage("Get PID");
    // balC.Print(&ua0);
  }
}

void CommandLoop()
{
  // printf("CommandLoop\n");
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 4) {
      balC.on = ua0.ReadI16();
      ua0.SvMessage2("RGL", balC.on);
    }
    if (cmd == 6) {
      calFlag=true; ua0.SvMessage("Cal start");
      imuGyro.CalGyro();
      kalm.Reset();
      calFlag=false; ua0.SvMessage("Cal done!!");
    }
    if (cmd == 7) { // 1..small -> 5..big  0..off
      int val = ua0.ReadI16();
      encL.setBW(val); encR.setBW(val);
      ua0.SvPrintf("BW: %d", val);
    }
    if (cmd == 8) {
      float offs = ua0.ReadF();
      if (offs != -1)
        angleOffs = offs;
      ua0.SvPrintf("angleOffs: %1.1f", angleOffs);
    }
    if (cmd == 9) {
      int val = ua0.ReadI16();
      limL.SetRateSec(val); limR.SetRateSec(val);
      ua0.SvMessage("SetRate");
    }
    if (cmd == 50) {
      esp_restart();
    }
    if (cmd == 55) {
      float turn = (float)ua0.ReadI16() * 0.5;
      float forew = (float)ua0.ReadI16() * 2.0;
      limL.in = -forew + turn;
      limR.in = -forew - turn;
      // ua0.SvMessage("stk");
    }
    if (cmd == 200 || cmd == 201)
      ControlParams(cmd);
  }
}

extern "C" void RPM_Task(void* arg)
{
  limL.CalcOneStep(); limR.CalcOneStep();
  // encL.CalcFilt2(); encR.CalcFilt2(); 2-chan Encoder
  encL.CalcFilt(); encR.CalcFilt(); // 1-chan Encoder
  DoDisplay();
}


// accY gyroX x,y,z  0,1,2
extern "C" void RglTask(void* arg)
{
  if (calFlag) 
    return;
  mpu.getAccelY(); mpu.getGyroX();
  imuAcc.CalcFilt(1); imuGyro.CalcFilt(0);
  kalm.CalcFilter(imuAcc.getFilt2(1), imuGyro.getFilt2(0));

  // if (encL.getFrequUS() > 100) encL.syncDir();
  balC.CalcOneStep(kalm.complAngle+angleOffs, encL.getFrequF());
  if( balC.on && kalm.getAbsAngle()<300 )
  {
    float fwL=limL.out*1E-3; float fwR=limR.out*1E-3;
    motL.setPow2(balC.pow+fwL); motR.setPow2(balC.pow+fwR);
  }
  else {
    balC.on = 0;
    motL.setPow2(0); motR.setPow2(0);
  }
}


void DoDisplay()
{
  if (ua0.acqON) {
    LockOStream();
    ua0.WriteSvF(1, encL.getFrequF());
    ua0.WriteSvF(2, encL.getFrequ());
    ua0.WriteSvI16(3, kalm.complAngle);
    // ua0.WriteSvI16(4, motL.pow*100); // +- 100
    // ua0.WriteSvI16(5, limL.out*0.1); // +- 100
    ua0.Flush();
    UnlockOStream();
  }
}


extern "C" void app_main(void)
{
  printf("SegWay_1\n");
  InitRtEnvHL(); printf("after InitRT\n");
  I2cInit(); printf("Conn: %X\n", mpu.testConnection()); mpu.Init();
  InitIO();
  MyDelay(100); InitUart(UART_NUM_0, 500000); MyDelay(100);
  // configMAX_PRIORITIES = 25
  EspTimSetup(300, RPM_Task, false);
  EspTimSetup(1000, RglTask, false);
  CommandLoop();
}
