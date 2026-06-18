
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "MotorSetup.h"
#include "TTMailBox.h"
SvProtocol3 ua0;
#include "RpmPID.h"

TTMailBox drCmd;

// 160 1100
void CommandLoop()
{
  // printf("CommandLoop\n");
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) {
      motL.setPow2(ua0.ReadF()); motR.setPow2(ua0.ReadF());
      ua0.SvMessage("Set Pow");
    }
    else if (cmd == 3) {
      rpmCL.setDemand(ua0.ReadI16());
      rpmCR.setDemand(ua0.ReadI16());
      ua0.SvMessage("Set demand");
    }
    else if (cmd == 4) {
      rpmCL.reset(); rpmCR.reset();
      rpmPara.on = ua0.ReadI16();
      if (!rpmPara.on) {
        vTaskDelay(3);
        motL.setPow2(0.0); motR.setPow2(0.0);
      }
      ua0.SvMessage2("rgl ", rpmPara.on);
    }
    else if (cmd == 5) {
      encL.cnt = encR.cnt = 0;
      ua0.SvMessage("Reset Cnt");
    }
    else if (cmd == 7) {
      int val = ua0.ReadI16();
      encL.setBW(val); encR.setBW(val);
      ua0.SvPrintf("BW: %d", val);
    }
    else if (cmd == 10) { // IPC msg to DriveTask
      drCmd.send(ua0.ReadI16());
      // DriveTask takes Parameters from SvMessage
      vTaskDelay(5);
    }
    else if (cmd == 11) { // cancel msg to DriveTask
      drCmd.set(-1);
    }
    else if (cmd == 200 || cmd == 201)
      ControlParams(cmd);
  }
}

// min Frequ 50
void DoDisp()
{
  if (!ua0.acqON)
    return;
  ua0.WriteSvF(1, rpmCL.getActDemand());
  ua0.WriteSvF(2, encL.getFrequF());
  ua0.WriteSvF(3, encR.getFrequF());
  ua0.WriteSvI16(4, encL.cnt);
  ua0.Flush3(6);
}

extern "C" void RtTask(void* arg)
{
  encL.CalcFilt2(); encR.CalcFilt2();
  if (rpmPara.on) {
    rpmCL.calcOneStep(encL.getFrequF());
    rpmCR.calcOneStep(encR.getFrequF());
    motL.setPow2(rpmCL.out); 
    motR.setPow2(rpmCR.out);
  } 
  DoDisp();
}


