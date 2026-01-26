
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "SvProtocol3.h"
#include "lwip/sockets.h"
#include "NodeLock.h"
#include "Features.h"
#include "LedStripHL.h"

void MyDelay(int aMSec);

#define UA_BUF_SIZE (1024)

#ifdef USE_UDP
struct sockaddr_storage g_source_addr;
#define SOCK_LEN2  (sizeof(struct sockaddr_storage))
socklen_t g_socklen = SOCK_LEN2;
#endif
int svSock = 0;
uart_port_t gUaNum = UART_NUM_0;
SemaphoreHandle_t sem2 = 0;

void SvProtocol3::Init()
{
  sem2 = xSemaphoreCreateMutex();
}

void SvProtocol3::LockOStream()
{
  xSemaphoreTake(sem2, portMAX_DELAY);
}

void SvProtocol3::UnlockOStream()
{
  xSemaphoreGive(sem2);
}

void InitUart(uart_port_t uaNum, int aBaud)
{
  ChkFeature2(FEAT_SEM);
  vTaskDelay(10);
  gUaNum = uaNum;
  uart_config_t uart_config = {
    .baud_rate = aBaud,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    // .rx_flow_ctrl_thresh = 0, S2
    .source_clk = UART_SCLK_DEFAULT,
  };
  const int intr_alloc_flags = 0;
  // const int intr_alloc_flags = ESP_INTR_FLAG_IRAM;
  ESP_ERROR_CHECK(uart_driver_install(uaNum, UA_BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
  ESP_ERROR_CHECK(uart_param_config(uaNum, &uart_config));
  if (uaNum == UART_NUM_0) {
    ESP_ERROR_CHECK(uart_set_pin(uaNum, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  }
  if (uaNum == UART_NUM_2) {  // S2
    ESP_ERROR_CHECK(uart_set_pin(uaNum, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  }
  vTaskDelay(10);
}

// esp_err_t uart_get_buffered_data_len(uart_port_t uart_num, size_t* size);


void cycleBlink(int rateReduce)
{
  static int cnt = 0;
  if (++cnt > rateReduce) {
    cnt = 0;
    leds.toggle(6,7, H_ORANGE);
    leds.refresh();
  }
}


void SvProtocol3::WriteIO(void* aData, size_t aLen)
{
#ifdef USE_UDP
  int64_t t1 = esp_timer_get_time();
  sendto(svSock, aData, aLen, 0, (struct sockaddr*)&g_source_addr, SOCK_LEN2);
  sendTim = esp_timer_get_time() - t1;
  if (sendTim > maxSendTim)
    maxSendTim = sendTim;
#else
  uart_write_bytes(gUaNum, aData, aLen);
#endif
}

int SvProtocol3::ReadIO(void* aBuff, uint32_t aLen)
{
#ifdef USE_UDP
  return recvfrom(svSock, aBuff, aLen, 0, (struct sockaddr*)&g_source_addr, &g_socklen);
#else
  return uart_read_bytes(gUaNum, aBuff, aLen, portMAX_DELAY);
#endif
}


void UdpRd::ReadMsg()
{
  rdIdx = 0;
#ifdef USE_UDP
  rdLen=recvfrom(svSock, buff, BUFF_SZ-2, 0, (struct sockaddr*)&g_source_addr, &g_socklen);
#endif
}


int SvProtocol3::GetCommand()
{
#ifdef USE_UDP
  udp.ReadMsg();
  uint8_t cmd = udp.ReadByte();
#else
  uint8_t cmd = ReadByte();
#endif
  if (cmd == 1) {
    acqON = ReadByte();
    if (resetFlag) {
      resetFlag = 0;
      SvMessage("Reset!!");
    }
    if (acqON)
      SvMessage("AcqON");
    else
      SvMessage("AcqOFF");
    return 0;
  }
  if (cmd == 51) {
    WrF(1.23456); // NodeLock Token
    Flush();
    return 0;
  }
  return cmd;
}



const int N_PRINT_BUFF = 128;

void SvProtocol3::SvPrintf(const char* format, ...)
{
  char locBuff[N_PRINT_BUFF];
  char* ptr = locBuff;
  *ptr = 10; ptr++;
  va_list vArgs;
  va_start(vArgs, format);
  vsprintf(ptr, (char const*)format, vArgs);
  va_end(vArgs);
  int len = strlen(locBuff); locBuff[len] = '\n';
  WriteIO(locBuff, len + 1);
}

void SvProtocol3::SvMessage(const char* aTxt)
{
  char locBuff[N_PRINT_BUFF];
  char* ptr = locBuff;
  *ptr = 10; ptr++;
  strcpy(ptr, aTxt);
  int len = strlen(locBuff); locBuff[len] = '\n';
  WriteIO(locBuff, len + 1);
}

void SvProtocol3::SvMessage2(const char* aTxt, bool aOnOff)
{
  char locBuff[N_PRINT_BUFF];
  char* ptr = locBuff;
  *ptr = 10; ptr++;
  strcpy(ptr, aTxt);
  if (aOnOff)
    strcat(ptr, " ON");
  else
    strcat(ptr, " OFF");
  int len = strlen(locBuff); locBuff[len] = '\n';
  WriteIO(locBuff, len + 1);
}

SvProtocol3& SvProtocol3::WrStr(const char* aStr)
{
  int len = strlen(aStr);
  uint8_t* ptr = buff + idx;
  strcpy((char*)ptr, aStr);
  *(ptr + len) = '\n';
  idx += len + 1;
  return *this;
}


extern "C" {
int wifi_init_sta();
void wifi_init_softap(char* aName, int aChan);
void print_own_ip_addr();
int InitUdp();
}

void InitStation()
{
#ifdef USE_UDP
  // InitNVS(); done in InitRtEnvHL()
  if (-1 == wifi_init_sta()) MyError("wifi");
  svSock = InitUdp();
  if (svSock == -1) MyError("sock");
#endif
}

void InitSoftAp(const char* aName, int aChan)
{
#ifdef USE_UDP
  // InitNVS(); done in InitRtEnvHL()
  wifi_init_softap((char*)aName, aChan);
  print_own_ip_addr();
  svSock = InitUdp();
  if (svSock == -1) MyError("sock");
#endif
}

extern "C" void UdpConnCB(int val)
{

}
