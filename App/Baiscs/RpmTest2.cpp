
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "MotorSetup.h"

SvProtocol3 ua0;
StopWatch stw;
int dispMode = 3;
RateLim limL(200), limR(200);

// 160 1100

void CommandLoop()
{
  // printf("CommandLoop\n");
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) {
      limL.in = ua0.ReadI16(); limR.in = ua0.ReadI16();
      ua0.SvMessage("Set Pow Ramp");
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
      ua0.SvPrintf("dt %d", (uint32_t)stw.dt);
    }
    else if (cmd == 6) {
      dispMode = ua0.ReadI16();
      ua0.SvMessage("set DispMode");
    }
    else if (cmd == 7) {
      int val = ua0.ReadI16();
      encL.setBW(val); encR.setBW(val);
      ua0.SvPrintf("BW: %d", val);
    }
    else if (cmd == 8) {
      int val = ua0.ReadI16();
      limL.SetRateSec(val); limR.SetRateSec(val);
      ua0.SvMessage("SetRate");
    }
    else if (cmd == 50) {
      esp_restart();
    }
  }
}

// min Frequ 50

void DoDisp()
{
  if (ua0.acqON) {
    Encoder2* enc = &encL;
    RateLim* lim = &limL;
    if (dispMode == 2) {
      enc = &encR; lim = &limR;
    } 
    if (dispMode<3) { // 1,2
      ua0.WriteSvF(1, enc->getFrequ());
      ua0.WriteSvF(2, enc->getFrequF());
      ua0.WriteSvI16(3, lim->out); // power
    }
    else if (dispMode == 3) { // 3
      ua0.WriteSvF(1, encL.getFrequF());
      ua0.WriteSvF(2, encR.getFrequF());
      ua0.WriteSvI16(3, limL.out);
    }
    else if (dispMode == 4) { // 4
      ua0.WriteSvI16(1, encL.getPW());
      ua0.WriteSvI16(2, encR.getPW());
    }
    else if (dispMode == 5) { // 5
      ua0.WriteSvI16(1, encL.cnt);
      ua0.WriteSvI16(2, encR.cnt);
    }
    ua0.Flush();
  }
}

extern "C" void RtTask(void* arg)
{
  stw.val(); stw.Reset();
  limL.CalcOneStep(); limR.CalcOneStep();
  motL.setPow2(limL.out / 1000); motR.setPow2(limR.out / 1000);
  encL.CalcFilt2(); encR.CalcFilt2();
  encL.CalcDiff(encL.getFrequF());
  encR.CalcDiff(encR.getFrequF());
  DoDisp();
}


extern "C" void app_main(void)
{
  printf("RpmTest2_1\n");
  InitRtEnvHL(); 
  InitMotors();
  InitUart(UART_NUM_0, 500000);
  // InitSoftAp("sepp", 5);
  EspTimSetup(300, RtTask, 0, false);
  CommandLoop();
}

