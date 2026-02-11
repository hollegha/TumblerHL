
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "LineController.h"

LsPololu ls;
// 0.6 sens back
// 1.0 sens front lilla
LinePID lpd(1.0, 0.0, 0.0, 0.0, "LinePD");

extern Motor motL, motR;
extern Encoder2 encL, encR;
extern GpIoOut stdby;

TTMailBox drCmd;
RangeClip<float> rg(0.1, 0.95);


extern "C" void RglTask(void* arg);
extern "C" void CommandLoop(void* arg);

void InitLineControler()
{
  Adc1::atten = ADC_ATTEN_DB_11;
  ls.initADC();
  if( ESP_OK!=ls.nvs2cal() ) // ls.setDefaultCalib();
    ls.initCal();
  ls.mode = CAL_VALS;
  lpd.onOff(false);
  // xTaskCreatePinnedToCore(Monitor, "Mon", 2048, NULL, 10, NULL, 1);
  EspTimSetup(200, RglTask, NULL, false);
  xTaskCreate(CommandLoop, "Cmd", 2048, NULL, 8, NULL); 
}

void LinePID::calcOneStep()
{
  const float DT = 0.005;
  float pos = ls.pos();
  if( KI!=0 )
    sum += pos;
  out = pos * KP + ls.posDiff() * KD + sum * KI * DT;
  out = -out; // ls-back
}

void setDispMode(int mode)
{
  if (mode == DISP_RAW) {
    ua0.SvMessage("disp raw");
    ls.mode = mode;
  }
  else if (mode == CALIBRATE) {
    ua0.SvMessage("calibarion...");
    ls.initCal(); // reihenfolge !!!
    ls.mode = mode;
  }
  else if (mode == CAL_VALS) {
    ua0.SvMessage("cal vals");
    ls.mode = mode;
  }
  else if (mode == ENCODERS) {
    ua0.SvMessage("disp ENC");
    ls.mode = mode;
  }
}

void ExecLineSensor();
void ControlParams(int cmd);


extern "C" void CommandLoop(void* arg)
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 3)
      setDispMode(ua0.ReadI16());
    else if (cmd == 4)
      ls.dispMinMax(&ua0);
    else if (cmd == 5) {
      ls.initCal();
      ua0.SvMessage("Init cal");
    }
    else if (cmd == 6) {
      float fg = ua0.ReadF();
      ls.tp.SetAlpha(fg);
      ua0.SvMessage("set Alpha");
    }
    else if (cmd == 7) { // RGL on off
      lpd.onOff(ua0.ReadI16());
      vTaskDelay(3);
      motL.setPow2(0); motR.setPow2(0);
      ua0.SvMessage2("RGL ", lpd.on);
    }
    else if (cmd == 9) { // IPC msg to DriveTask
      drCmd.send(ua0.ReadI16());
      // DriveTask takes Parameters from SvMessage
      vTaskDelay(5); 
    }
    else if (cmd == 10) {
      lpd.forew = ua0.ReadF();
      ua0.SvPrintf("forew %1.2f", lpd.forew);
    }
    else if (cmd == 11) {
      int val = ua0.ReadI16();
      stdby.Set(val);
      ua0.SvMessage2("stdby", val);
    }
    else if (cmd == 12) { // reset ENC
      encL.cnt = encR.cnt = 0;
    }
    else if (cmd == 13) { // cal2NVS
      ls.cal2nvs();
      ua0.SvMessage("cal -> NVS");
    }
    else if (cmd == 14) { 
      ls.nvs2cal();
      ua0.SvMessage("NVS -> cal");
    }
    else if (cmd == 15) { // send Time
      int flag = ua0.ReadI16();
      ua0.SvPrintf("st %d %d", ua0.sendTim, ua0.maxSendTim);
      ua0.maxSendTim = 0;
    }
    else if (cmd == 16) {
      encL.cnt = encR.cnt = 0;
      ua0.SvMessage("reset enc");
    }
    else if (cmd == 200 || cmd == 201)
      ControlParams(cmd);
    else if (cmd == 50) {
      esp_restart();
    }
  }
}


void ControlParams(int cmd)
{
  if (cmd == 200) {
    int num = ua0.ReadI16();
    lpd.ReadCOM(&ua0);
    ua0.SvMessage("Set PID");
  }
  if (cmd == 201) {
    int num = ua0.ReadI16();
    ua0.LockOStream();
    ua0.WrB(200);
    lpd.WriteCOM(&ua0);
    ua0.Flush();
    ua0.UnlockOStream();
    ua0.SvMessage("Get PID");
  }
}

void Display()
{
  if (!ua0.acqON)
    return;
  ua0.LockOStream();
  if (ls.mode == DISP_RAW) {
    ls.dispRawVals(&ua0);
  }
  else if (ls.mode == CALIBRATE) {
    ls.dispYVals(&ua0);
  }
  else if (ls.mode == CAL_VALS) {
    ls.dispYVals(&ua0);
    ua0.WriteSvI16(7, ls._pos);
    // ua0.WriteSvI16(8, ls._posDiff);
  }
  else if (ls.mode == ENCODERS) {
    ua0.WriteSvI16(1, (int)encL.cnt);
    ua0.WriteSvI16(2, (int)encR.cnt);
  }
  ua0.Flush3(4);
  ua0.UnlockOStream();
  // cycleBlink(20);
}

extern "C" void RglTask(void* arg)
{
  ExecLineSensor();
  Display();
}

void ExecLineSensor()
{
  ls.readADC();
  if (ls.mode == CALIBRATE) {
    ls.calcCalFilt();
    ls.calStep();
  }
  else if (ls.mode == CAL_VALS) {
    ls.calcCal();
    ls.calcPos2();
  }
  if (lpd.on) {
    /* if (!ls.posInRange()) {
      lpd.onOff(false);
      motL.setPow2(0); motR.setPow2(0);
      return;
    } */
    lpd.calcOneStep();
    if (lpd.forew != 0) {
      motL.setPow2(rg.Clip(lpd.forew + lpd.out));
      motR.setPow2(rg.Clip(lpd.forew - lpd.out));
    }
    else {
      motL.setPow2(lpd.out); motR.setPow2(-lpd.out);
    }
  }
}

