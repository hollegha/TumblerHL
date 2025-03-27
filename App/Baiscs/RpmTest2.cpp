
#include "RTEnvHL.h"
#include "SvProtocol2.h"
#include "EspMotor.h"
#include "driver/gpio.h"

SvProtocol2 ua0;
StopWatch stw;
int dispMode = 3;

const int PWA = 4, AI1 = 32, PWB = 33, BI1 = 5, ST_BY = 18;
const int M2A = 19, M1A = 23, M2B = 36, M1B = 39;

Motor motL(PWA, AI1, -1); // -1==Tumbler Motor
Motor motR(PWB, BI1, -1);
GpIoOut stdby(ST_BY);
// Encoder encL(M2A, -1, &motL), encR(M1A, -1, &motR); 1-Chan Encoder
Encoder encL(M2A, M2B, &motL), encR(M1A, M1B, &motR); // 2-Chan Encoder

RateLim limL(200), limR(200);

// 160 1100

void InitIO()
{
  stdby.Init(); stdby.Set(1);
  Motor::InitTimer(); motR.Init(); motL.Init(); Motor::StartTimer();
  motR.setPow2(0.0); motL.setPow2(0.0);
  encL.Init(); encR.Init(); encR.inv = true;
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
  motL.setPow2(limL.out / 1000); motR.setPow2(limR.out / 1000);
  encL.CalcFilt2(); encR.CalcFilt2();
  encL.CalcDiff(encL.getFrequF());
  encR.CalcDiff(encR.getFrequF());
  DoDisp();
}


extern "C" void app_main(void)
{
  printf("RpmTest2_3\n");
  InitRtEnvHL(); 
  InitIO();
  MyDelay(100); InitUart(UART_NUM_2, 115200); MyDelay(100); // 115200
  // xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL);
  EspTimSetup(300, RtTask, false);
  CommandLoop();
}
