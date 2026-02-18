
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "MotorSetup.h"
#include "TmbMenues.h"

SvProtocol3 ua0;

void CommandLoop()
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) {
      motL.setPow2(ua0.ReadF()); motR.setPow2(ua0.ReadF());
      ua0.SvMessage("Set Pow");
    }
    if (cmd == 3) {
      encL.cnt = encR.cnt = 0;
      ua0.SvMessage("reset enc");
    }
    if (cmd == 4) {
      int v1 = ua0.ReadI16();
      float v2 = ua0.ReadF();
      ua0.SvPrintf("para demo  %d  %1.2f", v1, v2);
    }
  }
}

extern "C" void RtTask(void* arg)
{
  encL.CalcFilt2(); encR.CalcFilt2();
  ua0.WriteSvF(1, encL.getFrequF());
  ua0.WriteSvF(2, encR.getFrequF());
  ua0.Flush();
}

extern "C" void app_main(void)
{
  printf("PatzDemo\n");
  InitRtEnvHL(); 
  InitMotors(); 
  InitTmbMenue();
  InitUart(UART_NUM_0, 500000);
  // InitSoftAp("sepp", 5);
  EspTimSetup(300, RtTask, 0, false);
  CommandLoop();
}
