
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <lwip/netdb.h>

#define WIFI_SSID      "OnePlus Nord CE 2 Lite 5G"
#define WIFI_PASS      "a3i3y89i"
#define MAXIMUM_RETRY 5

static EventGroupHandle_t s_wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define PORT 3333

static const char* TAG = "stat";

static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
  int32_t event_id, void* event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (s_retry_num < MAXIMUM_RETRY) {
      esp_wifi_connect();
      s_retry_num++;
      printf("retry to connect to the AP\n");
    }
    else {
      xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }
    printf("connect to the AP fail\n");
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
    printf("got ip: %d.%d.%d.%d\n", IP2STR(&event->ip_info.ip));
    s_retry_num = 0;
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

int wifi_init_sta()
{
  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
    ESP_EVENT_ANY_ID,
    &event_handler,
    NULL,
    &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
    IP_EVENT_STA_GOT_IP,
    &event_handler,
    NULL,
    &instance_got_ip));

  wifi_config_t wifi_config = {
      .sta = {
          .ssid = WIFI_SSID,
          .password = WIFI_PASS,
          /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
           * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
           * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
           * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
           */
          .threshold.authmode = WIFI_AUTH_WPA2_PSK,
          .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
          .sae_h2e_identifier = "",
      },
  };
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  // printf("first disconnect\n");
  // esp_wifi_disconnect();
  // vTaskDelay(100);
  ESP_LOGI(TAG,"wifi_init_sta finished.");

  /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
   * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
    WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
    pdFALSE,
    pdFALSE,
    portMAX_DELAY);

  /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
   * happened. */
  int ret = 0;
  if (bits & WIFI_CONNECTED_BIT) {
    printf("connected to ap SSID:%s password:%s\n",
      WIFI_SSID, WIFI_PASS); ret=0;
  }
  else if (bits & WIFI_FAIL_BIT) {
    printf("Failed to connect to SSID:%s, password:%s\n",
      WIFI_SSID, WIFI_PASS); ret=-1;
  }
  else {
    printf("UNEXPECTED EVENT\n"); ret = -1;
  }
  return ret;
}

int InitUdp()
{
  int ip_protocol = 0;
  struct sockaddr_in6 dest_addr;

  // if (addr_family == AF_INET) 
  struct sockaddr_in* dest_addr_ip4 = (struct sockaddr_in*)&dest_addr;
  dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
  dest_addr_ip4->sin_family = AF_INET;
  dest_addr_ip4->sin_port = htons(PORT);
  ip_protocol = IPPROTO_IP;

  int sock = socket(AF_INET, SOCK_DGRAM, ip_protocol);
  if (sock < 0) {
    ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    return -1;
  }
  // printf("Socket created\n");

  // Set timeout
  struct timeval timeout;
  timeout.tv_sec = 100000;
  timeout.tv_usec = 0;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

  int err = bind(sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
  if (err < 0) {
    ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    return -1;
  }
  // printf("Socket bound, port %d\n", PORT);
  return sock;
}

void wifi_disconnect_impl()
{
  esp_wifi_disconnect();
  esp_wifi_stop();
}








