
#include "RTEnvHL.h"
#include "TmbMenues.h"


extern "C" void app_main(void)
{
  printf("MenueTest_1\n");
  InitRtEnvHL(); InitTmbMenue();
  
  int sel = 0;
  while (1) 
  {
    sel = execMenue();
    printf("sel %d working......\n", sel);
    MyDelay(3000);
  }
}


