
#include "RTEnvHL.h"
#include "SvProtocol3.h"
#include "EspMotor.h"
#include "LineController.h"
#include "TmbMenues.h"
#include "MotorSetup.h"

extern LsPololu ls;
extern LinePID lpd;
SvProtocol3 ua0;


extern "C" void app_main(void)
{
  printf("MinLineApp_1\n");
  InitRtEnvHL();
  InitMotors();
  InitLineSensor();
  InitUart(UART_NUM_0, 500000); 
  // InitSoftAp("franz_1", 1);
  InitLineControler();
  // print_task_list();
  while(1)
    MyDelay(10000);
}











