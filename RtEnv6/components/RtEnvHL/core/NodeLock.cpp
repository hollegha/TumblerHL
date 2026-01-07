
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_mac.h"
#include "NodeLock.h"

nvs_handle_t nvsRtEnv = 0; 

uint32_t gFeatures = 0;

void fls()
{
  fflush(stdout);
}

void MyError(const char* aMsg)
{
  printf("Error: %s\n", aMsg); fls();
  while (1)
    vTaskDelay(100);
}

void PrintMac(const char* aTxt, uint8_t addr[])
{
  printf("%s: ", aTxt);
  for (int i = 0; i < 6; i++)
    printf("%02X:", addr[i]);
  printf("\n"); fls();
}

void InitNVS()
{
  printf("InitNVS\n");
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
    printf("!!NVS erased!!\n");
  }
  ESP_ERROR_CHECK(ret);
}

void openRtEnvStore()
{
  // ESP_ERROR_CHECK(nvs_flash_init()); once in InitRtEnv()
  ESP_ERROR_CHECK(nvs_open("esp1", NVS_READWRITE, &nvsRtEnv));
}

void closeRtEnvStore()
{
  nvs_close(nvsRtEnv);
}

void Mac2NVS(uint32_t aFeatures)
{
  uint8_t addr[6];
  ESP_ERROR_CHECK(esp_efuse_mac_get_default(addr));
  PrintMac("mac->nvs", addr);
  ESP_ERROR_CHECK(nvs_set_blob(nvsRtEnv, "esp2", addr, 6));
  ESP_ERROR_CHECK(nvs_set_u32(nvsRtEnv, "esp3", aFeatures));
  ESP_ERROR_CHECK(nvs_commit(nvsRtEnv));
}

void ShowMacAll()
{
  uint8_t addr[10];
  size_t len;
  MY_ERR1(nvs_get_blob(nvsRtEnv, "esp2", addr, &len));
  MY_ERR1(nvs_get_u32(nvsRtEnv, "esp3", &gFeatures));
  PrintMac("NVS", addr);
  ESP_ERROR_CHECK(esp_efuse_mac_get_default(addr));
  PrintMac("MAC", addr);
  printf("Feat: %04X\n", (unsigned int)gFeatures);
}

void CheckNodeLock()
{
  uint8_t addr1[10], addr2[10];
  size_t len;
  MY_ERR1(nvs_get_blob(nvsRtEnv, "esp2", addr1, &len));
  MY_ERR1(nvs_get_u32(nvsRtEnv, "esp3", &gFeatures));
  ESP_ERROR_CHECK(esp_efuse_mac_get_default(addr2));
  for (int i = 0; i < 6; i++)
    if (addr1[i] != addr2[i])
      MyError("MacAddr");
}

void EraseNVS()
{
  if (ESP_OK != nvs_flash_erase())
    MyError("EraseNVS");
  else {
    printf("Erase OK\n"); fls();
  }
}

void ShowMac()
{
  uint8_t addr[6];
  ESP_ERROR_CHECK(esp_efuse_mac_get_default(addr));
  PrintMac("SMac", addr);
}



