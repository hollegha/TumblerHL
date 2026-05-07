
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "MotorSetup.h"
#include "MPU_Esp.h"
#include "ControlBasics.h"

SvProtocol3 ua0;

class MyController : public PIDParam {
public:
  MyController(float aKP, float aKD, const char* aName)
    : PIDParam(aKP, aKD, 0.0, 0.0, aName)
  {
  }
};

MyController rgl(2.0E-2, 3.0E-4, "myContr");


void CommandLoop()
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) { // mot dir test
      motL.setPow2(ua0.ReadF()); motR.setPow2(ua0.ReadF());
      ua0.SvMessage("set pow L/R");
    }
    // fixed commands from SvVis
    else if (cmd == 200 || cmd == 201)
      setGetControlParams(cmd, &ua0, &rgl);
  }
}

void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1); // 100Hz
    if (ua0.acqON) {
      ua0.LockOStream();
      ua0.WriteSvI16(1, encL.cnt);
      ua0.WriteSvI16(2, encR.cnt);
      ua0.Flush();
      ua0.UnlockOStream();
    }
  }
}

extern "C" void app_main(void)
{
  printf("ContParam\n");
  InitRtEnvHL(); InitMotors();
  initMPU();
  InitUart(UART_NUM_0, 500000);
  // InitSoftAp("sepp", 5);
  createTask(Monitor, "Monitor", 2048, 10);
  CommandLoop();
}
