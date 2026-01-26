
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "LineSensor.h"

// GpIoOut irLED(26);
SvProtocol3 ua0;
LsPololu ls;

void InitLineSensor()
{
  Adc1::atten = ADC_ATTEN_DB_11;
  ls.mode = DISP_RAW;
  ls.initADC();
  ls.initCal(); // ls.setDefaultCalib();
  ls.mode = DISP_RAW;
  // irLED.Init(); irLED.Set(1);
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
}

void CommandLoop()
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
    else if (cmd == 13) { // cal2NVS
      ls.cal2nvs();
      ua0.SvMessage("cal -> NVS");
    }
    else if (cmd == 14) {
      ls.nvs2cal();
      ua0.SvMessage("NVS -> cal");
    }
    else if (cmd == 50) {
      esp_restart();
    }
  }
}

void Display()
{
  if (!ua0.acqON)
    return;
  if (ls.mode == DISP_RAW) {
    ls.dispRawVals(&ua0);
  }
  else if (ls.mode == CALIBRATE) {
    ls.dispYVals(&ua0);
  }
  else if (ls.mode == CAL_VALS) {
    ls.dispYVals(&ua0);
    ua0.WriteSvI16(7, ls._pos);
    ua0.WriteSvI16(8, ls._posDiff);
  }
  ua0.Flush();
}

extern "C" void Monitor(void* arg)
{
  int usCnt = 0;
  while (1) {
    vTaskDelay(1);
    ls.readADC();
    if (ls.mode == CALIBRATE) {
      ls.calcCalFilt();
      ls.calStep();
    }
    else if (ls.mode == CAL_VALS) {
      ls.calcCal();
      ls.calcPos2();
    }
    Display();
    if (ls.mode == CAL_VALS) {
      if (++usCnt >= 10) {
        usCnt = 0; ls.dispOnLeds();
      }
    }
  }
}

extern "C" void app_main(void)
{
  printf("FullLineSensTest_1\n");
  InitRtEnvHL();
  InitLineSensor();
  InitUart(UART_NUM_0, 500000);
  // InitSoftAp("sepp", 5);
  xTaskCreate(Monitor, "Mon", 2048, NULL, 10, NULL);
  CommandLoop();
}



