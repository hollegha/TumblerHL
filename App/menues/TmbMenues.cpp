
#include "TmbMenues.h"

ButtonHandler hbtn(2);
LedStripHL leds;

void InitTmbMenue()
{
  hbtn.Init();
  leds.Init();
}


void IRAM_ATTR button_isr(void* arg)
{
  ((ButtonHandler*)arg)->ISRFunction();
}

void ButtonHandler::Init()
{
  GpIoInitInterrupt(_pin, button_isr, this, GPIO_INTR_POSEDGE);
  _sema = xSemaphoreCreateBinary();
}

void ButtonHandler::ISRFunction()
{
  if (_waiting ) {
    _waiting = false;
    BaseType_t prioThings = pdFALSE;
    xSemaphoreGiveFromISR(_sema, &prioThings);
    portYIELD_FROM_ISR(prioThings);
  }
  cnt++;
}

void ButtonHandler::wait()
{
  while (1) {
    _waiting = true;
    xSemaphoreTake(_sema, portMAX_DELAY);
    vTaskDelay(10);
    if (check() == 1)
      return;
  }
}



static int mSel = 1;
static int cols[] = { H_RED, H_RED, H_YELLOW2, H_GREEN2, H_BLUE };

void showLED()
{
  leds.setHueFT(0, 1, cols[mSel]);
  leds.refresh(); 
}

void blinkLED()
{
  for (int i = 1; i <= 5; i++) {
    leds.setHueFT(0, 1, cols[mSel]); leds.refresh();
    MyDelay(100);
    leds.clear();
    MyDelay(100);
  }
}

int execMenue()
{
  leds.clear();
  mSel = 1;
  while (1) {
    hbtn.listen();
    showLED(); MyDelay(1200);
    if (hbtn.wasClicked())
      goto SelDone;
    mSel++;
    if (mSel > 4) mSel = 1;
  }
SelDone:
  blinkLED(); leds.clear();
  return mSel;
}


int execMenueDBL()
{
  leds.clear();
  mSel = 1;
  while (1) {
    showLED();
    hbtn.cnt=0; hbtn.wait();
    MyDelay(500);
    if (hbtn.cnt > 100)
      goto SelDone;
    mSel++;
    if (mSel > 4) mSel = 1;
  }
SelDone:
  blinkLED(); leds.clear();
  return mSel;
}








