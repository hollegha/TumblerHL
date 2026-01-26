
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "MotorSetup.h"
#include "TmbMenues.h"

SvProtocol3 ua0;
// trg echo
UsDist ds(5, 12);

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

void DriveDist(float pow, int aDist)
{
  encL.cnt = encR.cnt = 0;
  motR.setPow2(pow); motL.setPow2(pow);
  while (encL.cnt < aDist)
    vTaskDelay(1);
  motR.setPow2(0); motL.setPow2(0);
}

void menueTask(void* arg)
{
  int sel;
  while (1) {
    sel = execMenueDBL();
    if (sel == 1)
      DriveDist(0.3, 200);
    else if (sel == 2)
      sel = 0; // dummy
    else if (sel == 3)
      sel = 0; // dummy
  }
}

extern "C" void app_main(void)
{
  printf("PatzDemo\n");
  InitRtEnvHL(); 
  InitMotors(); ds.Init();
  InitTmbMenue();
  InitUart(UART_NUM_0, 500000);
  // InitSoftAp("sepp", 5);
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL);
  xTaskCreate(menueTask, "Menue", 2048, NULL, 10, NULL);
  CommandLoop();
}
