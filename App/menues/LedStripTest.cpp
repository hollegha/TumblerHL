
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "LedStripHL.h"

SvProtocol3 ua0;
LedStripHL ls;

int gHue = 10;

class HueRamp {
public:
  int val, step, offs;
public:
  HueRamp()
  {
    val = 0; step = 5; offs = 20;
  }
  void makeStep()
  {
    val += step;
    if (val > 360)
      val = 0;
  }
  int getOffs(int i)
  {
    int ret = val + i*offs;
    if (ret > 360)
      return 1;
    else
      return ret;
  }
  void calcHue()
  {
    makeStep();
    for(int i=0; i<8; i++)
      ls.setHue(i, getOffs(i));
  }
};

HueRamp ramp;


void CommandLoop()
{
  int cmd;
  while (1) {
    cmd = ua0.GetCommand();
    if (cmd == 2) { // RGB
      int16_t para[3]; ua0.ReadI16Ary(para, 3);
      for(int i=0; i<8; i++)
        ls.setRGB(i, para[0], para[1], para[2]);
      ls.refresh();
      ua0.SvPrintf("rbg %d %d %d", para[0], para[1], para[2]);
    }
    if (cmd == 3) { // sat val
      ls.sat = ua0.ReadI16(); ls.val = ua0.ReadI16();
      for (int i = 0; i < 8; i++)
        ls.setHue(i, gHue);
      ls.refresh();
      ua0.SvPrintf("sat val %d %d", ls.sat, ls.val);
    }
    if (cmd == 4) { // step offs
      ua0.rdI16(ramp.step).rdI16(ramp.offs);
      ua0.SvPrintf("step offs %d %d", ramp.step, ramp.offs);
    }
    if (cmd == 5) { // Hue
      gHue = ua0.ReadI16();
      for (int i = 0; i < 8; i++)
        ls.setHue(i, gHue);
      ls.refresh();
      ua0.SvPrintf("hue %d", gHue);
    }
    if (cmd == 6) { // clear
      ls.clear();
      ua0.SvMessage("clear");
    }
    if (cmd == 50) {
      esp_restart();
    }
  }
}

extern "C" void Monitor(void* arg)
{
  while (1) {
    vTaskDelay(10);
    ramp.calcHue(); ls.refresh();
    if (ua0.acqON) {
      ua0.WriteSvI16(1, ramp.val);
      ua0.Flush();
    }
  }
}

extern "C" void app_main(void)
{
  printf("LedStripTest_1\n");
  InitRtEnvHL(); ls.Init();
  InitUart(UART_NUM_0, 500000);
  xTaskCreate(Monitor, "Monitor", 2048, NULL, 10, NULL);
  CommandLoop();
}











