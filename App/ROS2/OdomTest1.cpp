
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "Odom.h"
#include "MotorSetup.h"
SvProtocol3 ua0;
#include "RpmPID.h"

int joyX=0, joyY=0;
RangeClip<float> rg2(0, 900);

void JoySteer();

// 160 1100
void CommandLoop()
{
  // printf("CommandLoop\n");
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) {
      motL.setPow2(ua0.ReadF()); motR.setPow2(ua0.ReadF());
      ua0.SvMessage("Set Pow");
    }
    else if (cmd == 3) {
      rpmCL.setDemand(ua0.ReadI16()); // rampe max setzen
      rpmCR.setDemand(ua0.ReadI16());
      ua0.SvMessage("Set demand");
    }
    else if (cmd == 4) {
      rpmCL.reset(); rpmCR.reset();
      rpmPara.on = ua0.ReadI16();
      if (!rpmPara.on) {
        vTaskDelay(3);
        motL.setPow2(0.0); motR.setPow2(0.0);
      }
      ua0.SvMessage2("rgl ", rpmPara.on);
    }
    else if (cmd == 5) {
      encL.cnt = encR.cnt = 0;
      odom.reset();
      ua0.SvMessage("Reset Cnt");
    }
    else if (cmd == 7) {
      int val = ua0.ReadI16();
      encL.setBW(val); encR.setBW(val);
      ua0.SvPrintf("BW: %d", val);
    }
    else if (cmd == 8) {
      int val = ua0.ReadI16();
      stdby.Set(val);
      ua0.SvMessage2("Stby ", val);
    }
    else if (cmd == 55) 
      JoySteer();
    else if (cmd == 200 || cmd == 201)
      ControlParams(cmd);
  }
}

void JoySteer()
{
  joyX = ua0.ReadI16(); joyY = ua0.ReadI16();
  float spL = joyY + 0.5*joyX;
  float spR = joyY - 0.5*joyX;
  rpmCL.setDemand(ClipHL(spL, 900));
  rpmCR.setDemand(ClipHL(spR, 900));
}

void DispRPM()
{
  if (!ua0.acqON)
    return;
  ua0.WriteSvF(1, rpmCL.getActDemand());
  ua0.WriteSvF(2, encL.getFrequF());
  ua0.WriteSvF(3, encR.getFrequF());
  ua0.WriteSvI16(4, encL.cnt);
  ua0.WriteSvI16(5, encR.cnt);
  ua0.Flush3(6); // udp schonen
}

extern "C" void RtTask(void* arg)
{
  encL.CalcFilt2(); encR.CalcFilt2();
  if (rpmPara.on) {
    rpmCL.calcOneStep(encL.getFrequF());
    rpmCR.calcOneStep(encR.getFrequF());
    motL.setPow2(rpmCL.out); 
    motR.setPow2(rpmCR.out);
  } 
  // DispRPM();
}

void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(1);
    odom.calcOdom();
    if (ua0.acqON) {
      ua0.WriteSvF(1, encL.getFrequF());
      ua0.WriteSvF(2, encR.getFrequF());
      ua0.WriteSvF(3, odom.getX());
      ua0.WriteSvF(4, odom.getY());
      ua0.WriteSvF(5, odom.phiGrad());
      ua0.Flush3(3);
    }
  }
}

extern "C" void app_main(void)
{
  printf("OdomTest1_7\n");
  InitRtEnvHL(); 
  InitMotors(); InitRpmPID();
  // InitUart(UART_NUM_0, 500000);
  InitSoftAp("sepp", 5);
  EspTimSetup(300, RtTask, 0, false);
  xTaskCreate(Monitor, "mon", 2048, NULL, 8, NULL);
  CommandLoop();
}

