
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "MotorSetup.h"
#include "TTMailBox.h"
SvProtocol3 ua0;
#include "GyroControl.h"

#define CHECK_CANCEL() if( drCmd.doCancel() ) goto StopAll

TTMailBox drCmd;
bool calFlag = false;

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
      calFlag = true;
      gyro.CalGyro(true);
      calFlag = false;
      integ.Reset();
      ua0.SvMessage("cal finished");
    }
    else if (cmd == 4) {
      integ.Reset();
      rgl.setDemand2(0);
      ua0.SvMessage("reset integ");
    }
    else if (cmd == 5) {
      rgl.on = ua0.ReadI16();
      if (!rgl.on) {
        rgl.forew = 0;
        vTaskDelay(2);
        motL.setPow2(0); motR.setPow2(0);
      }
      ua0.SvMessage2("rgl ", rgl.on);
    }
    else if (cmd == 6) {
      rgl.setDemand(ua0.ReadI16());
      ua0.SvMessage("demand");
    }
    else if (cmd == 7) {
      int val = ua0.ReadI16();
      stdby.Set(val);
      ua0.SvMessage2("stdby", val);
    }
    else if (cmd == 8) {
      rgl.forew = ua0.ReadF();
      ua0.SvMessage("forew");
    }
    else if (cmd == 10) { // IPC msg to DriveTask
      drCmd.send(ua0.ReadI16());
      // DriveTask takes Parameters from SvMessage
      vTaskDelay(5);
    }
    else if (cmd == 11) { // cancel msg to DriveTask
      drCmd.set(-1);
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
      ua0.WriteSvF(1, rgl.getActDemand());
      ua0.WriteSvF(2, integ.gySum);
      ua0.WriteSvF(3, encL.cnt);
      ua0.Flush();
      ua0.UnlockOStream();
    }
  }
}

// x, y, z  0, 1, 2
void RglTask(void* arg)
{
  if (calFlag)
    return;
  mpu.getGyro(); // read I2C
  gyro.CalcFilt(2);
  integ.CalcFilter(0, gyro.getFilt2(2));
  rgl.calcOneStep(integ.gySum);
  if (rgl.on) {
    motL.setPow2(-rgl.out + rgl.forew);
    motR.setPow2(rgl.out + rgl.forew);
  }
}


void StraightAndCurve(float forew, int dist)
{
  ua0.SvPrintf("drive %1.2f %d", forew, dist);
  MyDelay(500);
  while(1) {
    ua0.SvMessage("Staright");
    integ.Reset(); rgl.setDemand2(0); // no ramp
    rgl.on = 1; encL.cnt = 0;
    rgl.forew = forew;
    while (encL.cnt < dist) {
      CHECK_CANCEL();
      vTaskDelay(1);
    }
    ua0.SvMessage("Curve");
    integ.Reset(); rgl.setDemand(1800); // with ramp
    while (rgl.getActDemand() < 1800.0) {
      CHECK_CANCEL();
      vTaskDelay(1);
    }
  }
StopAll:
  drCmd.set(0); encL.cnt = 0;
  rgl.on=0; vTaskDelay(2);
  motL.setPow2(0); motR.setPow2(0);
  integ.Reset(); rgl.setDemand2(0);
}


void DriveTask(void* arg)
{
  int cmd;
  while (1) {
    cmd = drCmd.recv();
    if (cmd == 1) {
      StraightAndCurve(ua0.ReadF(), ua0.ReadI16());
      ua0.SvMessage("Curve finished!!");
    }
  }
}

extern "C" void app_main(void)
{
  printf("GyroControl_1\n");
  InitRtEnvHL(); InitMotors();
  initMPU();
  // InitUart(UART_NUM_0, 500000);
  InitSoftAp("sepp", 5);
  createTask(Monitor, "Monitor", 2048, 10);
  xTaskCreate(DriveTask, "drt", 2048, NULL, 8, &(drCmd.t_recv));
  EspTimSetup(1000, RglTask, 0, false);
  CommandLoop();
}
