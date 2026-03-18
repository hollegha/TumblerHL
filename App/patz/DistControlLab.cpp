
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "MotorSetup.h"
#include "TmbMenues.h"

SvProtocol3 ua0;
// trg echo
UsDist ds(5, 12);

float sollDist = 800.0;
float KP = 0.003;
int rglON = 0;

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
      sollDist = ua0.ReadF();
      ua0.SvPrintf("solDist %1.2f", sollDist);
    }
    if (cmd == 5) {
      KP = ua0.ReadF();
      ua0.SvPrintf("KP %1.4f", KP);
    }
    if (cmd == 6) {
      rglON = ua0.ReadI16();
      if (!rglON) {
        vTaskDelay(3); motL.setPow2(0); motR.setPow2(0);
      }
      ua0.SvMessage2("rgl ", rglON);
    }
  }
}

void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1); // 100Hz
    if (ua0.acqON) {
      ua0.WriteSvI16(1, encL.cnt);
      ua0.WriteSvI16(2, encR.cnt);
      ua0.WriteSvI16(3, ds.dist);
      ua0.WriteSvI16(4, ds.getFilt());
      ua0.Flush();
      ds.startMeas(); // startMeas() only in this task
    }
  }
}

void DistTask(void* arg)
{
  const float D_MIN = 100.0;
  const float D_MAX = 2000.0;
  while (1) {
    vTaskDelay(1); // 100Hz
    if (!rglON)
      continue;
    if (ds.getFilt() < D_MIN || ds.getFilt() > D_MAX) {
      motL.setPow2(0); motR.setPow2(0);
      continue;
    }
    float pow = KP * (sollDist - ds.getFilt());
    pow = -pow;
    motL.setPow2(pow); motR.setPow2(pow);
  }
}

void menueTask(void* arg)
{
  int sel;
  while (1) {
    sel = execMenueDBL();
    if (sel == 1)
      DistTask(0);
    else if (sel == 2)
      sel = 0; // dummy
    else if (sel == 3)
      sel = 0; // dummy
  }
}

extern "C" void app_main(void)
{
  printf("DistCntr_5\n");
  InitRtEnvHL(); 
  InitMotors(); ds.Init();
  InitTmbMenue();
  // InitUart(UART_NUM_0, 500000);
  InitSoftAp("sepp", 5);
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL);
  // xTaskCreate(menueTask, "Menue", 2048, NULL, 10, NULL);
  xTaskCreate(DistTask, "dist", 2048, NULL, 10, NULL);
  CommandLoop();
}
