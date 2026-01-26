
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "PosControl.h"
#include "MotorSetup.h"

SvProtocol3 ua0;

// Rate KP KD
PosController rglL(200.0, 0.1, 0.01, "PosC");
PosController rglR(rglL);

void ControlParams(int cmd)
{
  if (cmd == 200) {
    int num = ua0.ReadI16();
    rglL.ReadCOM(&ua0); rglR.Copy(rglL);
    ua0.SvMessage("Set PID");
  }
  if (cmd == 201) {
    int num = ua0.ReadI16();
    ua0.LockOStream();
    ua0.WrB(200);
    rglL.WriteCOM(&ua0);
    ua0.UnlockOStream();
    ua0.SvMessage("Get PID");
  }
}

void CommandLoop()
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) {
      rglL.setDemand(ua0.ReadI16()); 
      rglR.setDemand(ua0.ReadI16());
      ua0.SvMessage("Set Pos");
    }
    else if (cmd == 3) {
      stdby.Set(ua0.ReadI16());
      ua0.SvMessage2("StdBy", stdby.val);
    }
    else if (cmd == 4) {
      encL.cnt = encR.cnt = 0;
      ua0.SvMessage("Reset Cnt");
    }
    else if (cmd == 5) {
      rglL.on = ua0.ReadI16();
      if (!rglL.on)
      { motL.setPow2(0); motR.setPow2(0); }
      ua0.SvMessage2("Rgl:", rglL.on);
    }
    else if (cmd == 6) {
      int rate = ua0.ReadI16();
      rglL.setRate(rate); rglR.setRate(rate);
      ua0.SvPrintf("Rate: %d", rate);
    }
    else if (cmd == 50) {
      esp_restart();
    }
    if (cmd == 200 || cmd == 201)
      ControlParams(cmd);
  }
}

void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1);
    if (rglL.on) {
      rglL.CalcOneStep(encL.cnt); rglR.CalcOneStep(encR.cnt);
      motL.setPow2(rglL.pow); motR.setPow2(rglR.pow);
    }
    if (ua0.acqON) {
      ua0.LockOStream();
      ua0.WriteSvI16(1, rglL.getActDemand());
      ua0.WriteSvI16(2, encL.cnt);
      ua0.WriteSvI16(3, encR.cnt);
      ua0.WriteSvI16(4, 100*motL.pow);
      ua0.Flush();
      ua0.UnlockOStream();
    }
  }
}

extern "C" void app_main(void)
{
  printf("PosControl\n");
  InitRtEnvHL(); InitMotors();
  // InitUart(UART_NUM_0, 500000);
  InitSoftAp("sepp", 5);
  createTask(Monitor, "Monitor", 2048, 10);
  CommandLoop();
}
