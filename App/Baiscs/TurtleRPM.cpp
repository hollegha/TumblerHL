
#include "RTEnvHL.h"
#include "SvProtocol2.h"
#include "EspMotor.h"
#include "driver/gpio.h"

SvProtocol2 ua0;
StopWatch stw;
int dispMode = 3;

Motor motL(21, 18, 19), motR(22, 5, 23);
Encoder encL(16, 4, &motL), encR(13, 12, &motR);

RateLim limL(200), limR(200);

// 160 1100

void InitIO()
{
  Motor::InitTimer(); motR.Init(); motL.Init(); Motor::StartTimer();
  motR.setPow(0.0); motL.setPow(0.0);
  encL.Init(); encR.Init();
  printf("InitIO finished\n");
}

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
    else if (cmd == 55) {
      float turn = (float)ua0.ReadI16() * 0.25;
      float forew = (float)ua0.ReadI16() * 0.5;
      limL.in = -forew + turn;
      limR.in = -forew - turn;
      // ua0.SvMessage("stk");
    }
  }
}

// min Frequ 50

void DoDisp()
{
  if (ua0.acqON) {
    Encoder* enc = &encL;
    RateLim* lim = &limL;
    if (dispMode == 2) {
      enc = &encR; lim = &limR;
    } 
    if (dispMode<3) { // 1,2
      ua0.WriteSvI16(1, enc->getFrequ());
      ua0.WriteSvI16(2, enc->getFrequF());
      ua0.WriteSvI16(3, lim->out); // power
    }
    else if (dispMode == 3) { // 3
      ua0.WriteSvI16(1, encL.getFrequF());
      ua0.WriteSvI16(2, encR.getFrequF());
      ua0.WriteSvI16(3, limL.out);
    }
    else { // 4
      ua0.WriteSvI16(1, encL.getPW());
      ua0.WriteSvI16(2, encR.getPW());
    }
    ua0.Flush();
  }
}

extern "C" void RtTask(void* arg)
{
  stw.val(); stw.Reset();
  limL.CalcOneStep(); limR.CalcOneStep();
  motL.setPow(limL.out / 1000); motR.setPow(limR.out / 1000);
  encL.CalcFilt2(); encR.CalcFilt2();
  encL.CalcDiff(encL.getFrequF());
  encR.CalcDiff(encR.getFrequF());
  DoDisp();
}


extern "C" void app_main(void)
{
  printf("TurtleRPM_1\n");
  InitRtEnvHL(); 
  InitIO();
  MyDelay(100); InitUart(UART_NUM_0, 500000); MyDelay(100); // 115200
  // xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL);
  EspTimSetup(300, RtTask, false);
  CommandLoop();
}
