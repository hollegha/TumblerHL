
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "MotorSetup.h"
SvProtocol3 ua0;
#include "GyroControl.h"

void CommandLoop()
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) { // mot dir test
      motL.setPow2(ua0.ReadF()); motR.setPow2(ua0.ReadF());
      ua0.SvMessage("set pow L/R");
    }
    else if (cmd == 3) {
      ua0.SvMessage("cal start");
      gyro.CalGyro(false);
      integ.Reset();
      ua0.SvMessage("cal finished");
    }
    else if (cmd == 4) {
      integ.Reset();
      ua0.SvMessage("reset integ");
    }
    else if (cmd == 5) {
      rgl.on = ua0.ReadI16();
      if (!rgl.on) {
        vTaskDelay(2);
        motL.setPow2(0); motR.setPow2(0);
      }
      ua0.SvMessage2("rgl ", rgl.on);
    }
    else if (cmd == 6) {
      rgl.demand = ua0.ReadI16();
      ua0.SvMessage("demand");
    }
    else if (cmd == 200 || cmd == 201)
      ControlParams(cmd);
  }
}

void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1); // 100Hz
    if (ua0.acqON) {
      ua0.LockOStream();
      ua0.WriteSvF(1, integ.gySum);
      ua0.WriteSvF(2, rgl.abw);
      ua0.WriteSvF(3, rgl.diff);
      ua0.Flush();
      ua0.UnlockOStream();
    }
  }
}

// x, y, z  0, 1, 2
void RglTask(void* arg)
{
  mpu.getGyro(); // read I2C
  gyro.CalcFilt(2);
  integ.CalcFilter(0, gyro.getFilt2(2));
  rgl.calcOneStep(integ.gySum);
  if (rgl.on) {
    motL.setPow2(-rgl.out); motR.setPow2(rgl.out);
  }
}

extern "C" void app_main(void)
{
  printf("GyroControl\n");
  InitRtEnvHL(); InitMotors();
  initMPU();
  InitUart(UART_NUM_0, 500000);
  // InitSoftAp("sepp", 5);
  createTask(Monitor, "Monitor", 2048, 10);
  EspTimSetup(1000, RglTask, 0, false);
  CommandLoop();
}
