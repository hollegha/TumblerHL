
#include "RTEnvHL.h"
#include "TmbMenues.h"

void MenueTest()
{
  int sel = 0;
  while (1)
  {
    sel = execMenueDBL();
    printf("sel %d working......\n", sel);
    MyDelay(3000);
  }
}

void DblClickTest()
{
  int lcnt = 0;
  while (1)
  {
    hbtn.cnt = 0;
    hbtn.wait();
    MyDelay(500);
    printf("wt %d %d\n", lcnt++, hbtn.cnt);
  }
}


extern "C" void app_main(void)
{
  printf("MenueTest_3\n");
  InitRtEnvHL(); InitTmbMenue();
  MenueTest();
}


