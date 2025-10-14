
#include "RoboLib7.h"
#include "EspMotor.h"

const int PWA = 4, AI1 = 32, PWB = 33, BI1 = 5, ST_BY = 18;
const int M2A = 19, M1A = 23, M2B = 36, M1B = 39;

Motor motL(PWA, AI1, -1); // -1==Tumbler Motor
Motor motR(PWB, BI1, -1);
GpIoOut stdby(ST_BY);
Encoder encL(M2A, M2B, &motL), encR(M1A, M1B, &motR);

extern "C" void app_main(void)
{
  printf("MotTest\n");
  stdby.Init(); stdby.Set(0);
  Motor::InitTimer(); motR.Init(); motL.Init(); Motor::StartTimer();
  encL.Init(); encR.Init(); encR.inv=true;
  
  /* motR.setPow2(0.2); motL.setPow2(0.3);
  MyDelay(2000);
  motR.setPow2(0.0); motL.setPow2(0.0); */
  printf("Finished\n");

  while (1) {
    MyDelay(200);
    printf("%d  %d\n", (int)encL.cnt, (int)encR.cnt);
  }
}


