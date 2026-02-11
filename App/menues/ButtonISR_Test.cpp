
#include "RTEnvHL.h"


void button_isr(void* arg);

class ButtonHandler {
private:
  int _pin;
  SemaphoreHandle_t _sema;
  bool _waiting;
public:
  ButtonHandler(int aPin)
  {
    _pin = aPin; cnt = 0; _waiting = false;
  }
  void Init()
  {
    GpIoInitInterrupt(_pin, button_isr, this, GPIO_INTR_POSEDGE);
    _sema = xSemaphoreCreateBinary();
  }
  void ISRFunction()
  {
    if (_waiting ) {
      _waiting = false;
      BaseType_t prioThings = pdFALSE;
      xSemaphoreGiveFromISR(_sema, &prioThings);
      portYIELD_FROM_ISR(prioThings);
    }
    cnt++;
  }
  void wait()
  {
    while (1) {
      _waiting = true;
      xSemaphoreTake(_sema, portMAX_DELAY);
      vTaskDelay(10);
      if (check() == 1)
        return;
    }
  }
  int check()
  {
    return gpio_get_level((gpio_num_t)_pin);
  }
public:
  int cnt;
};

void IRAM_ATTR button_isr(void* arg)
{
  ((ButtonHandler*)arg)->ISRFunction();
}


ButtonHandler hb(2);

extern "C" void app_main(void)
{
  printf("ButtonISR_Test_1\n");
  InitRtEnvHL(); hb.Init();
  
  int lcnt = 0;
  while (1) 
  {
    hb.wait();
    printf("wt %d\n", lcnt++);
  }
}


