
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "LineController.h"
#include "MotorSetup.h"

extern LineSensor ls;
extern LinePID lpd;
SvProtocol3 ua0;

// #define CHECK_CANCEL() if( drCmd.doCancel() ) goto StopAll
// #define CHECK_CANCEL2() if( drCmd.doCancel() ) return

#define CHECK_CANCEL() if( btn.get() ) goto StopAll
#define CHECK_CANCEL2() if( btn.get() ) return


float ReadLineValues()
{
  float val;
  for (int i = 0; i < 5; i++) {
    val = lsAry[i].rawVal; // raw Value
    val = lsAry[i].y; // calibrated Value
  }
  val = ls.posNorm(); // position of Line +/- 1.0
  val = ls.pos3; // raw position value
  return val;
}


// Recv Commands from UI
extern "C" void UICommands(void* arg)
{
  int cmd;
  while (1) {
    cmd = drCmd.recv();
    if (cmd == 1) {
      float v1 = ua0.ReadF(), v2 = ua0.ReadF();
      ua0.SvPrintf("drc %1.1f %1.1f", v1, v2);
    }
    else if (drCmd.doCancel())
      drCmd.clear();
  }
}

void BtnDriveControl()
{
  while (1) {
    while (!btn.get())
      MyDelay(100);
    MyDelay(500); // Btn release
    lpd.forew=0.2; lpd.onOff(true); // follow Line
    MyDelay(2000); 
    lpd.forew=0; lpd.onOff(false); // stop follow
    vTaskDelay(3);
    motL.setPow2(0); motR.setPow2(0);
  }
}

extern "C" void app_main(void)
{
	printf("Template_LineApp\n");
  InitRtEnvHL();
  InitMotors(); InitLineControler();
  // InitUart(UART_NUM_0, 500000);
  InitSoftAp("sepp", 6);
  xTaskCreate(UICommands, "UI", 2048, NULL, 8, &(drCmd.t_recv));
  BtnDriveControl();
}











