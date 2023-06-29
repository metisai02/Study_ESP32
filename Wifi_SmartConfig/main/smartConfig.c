#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"

TaskHandle_t task1;
#define CONNECTED_BIT (1 << 0)
#define ESPTOUCH_DONE_BIT (1 << 4)
EventGroupHandle_t s_wifi_smartconfig;
static void smartConfig_handler(void *para);
static void
event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
            xTaskCreate(smartConfig_handler, "uart", 1024 * 3, NULL, 5, &task1);
            printf("WIFI_EVENT_STA_START\n");
            /* code */
            break;
        case WIFI_EVENT_STA_CONNECTED:
            printf("WIFI_EVENT_STA_CONNECTED\n");
            /* code */
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            esp_wifi_connect();
            xEventGroupClearBits(s_wifi_smartconfig, CONNECTED_BIT);
            printf("WIFI_EVENT_STA_DISCONNECTED\n");

            /* code */
            break;
        case WIFI_EVENT_STA_BEACON_TIMEOUT:
            printf("WIFI_EVENT_STA_CONNECTED\n");
            /* code */
            break;
        case WIFI_EVENT_STA_STOP:
            printf("WIFI_EVENT_STA_STOP\n");
            /* code */
            break;

        default:
            break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP:
            printf("IP_EVENT_STA_GOT_IP\n");
            xEventGroupSetBits(s_wifi_smartconfig, CONNECTED_BIT);
            break;
        default:
            break;
        }
    }
    else if (event_base == SC_EVENT)
    {
        switch (event_id)
        {
        case SC_EVENT_SCAN_DONE:
            printf("SC_EVENT_SCAN_DONE\n");
            break;
        case SC_EVENT_FOUND_CHANNEL:
            printf("SC_EVENT_FOUND_CHANNEL\n");
            break;
        case SC_EVENT_GOT_SSID_PSWD:
            uint8_t ssid[33] = {0};
            uint8_t password[65] = {0};
            uint8_t rvd_data[33] = {0};
            wifi_config_t wifi_config;
            smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
            bzero(&wifi_config, sizeof(wifi_config_t));
            memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
            memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
            memcpy(ssid, evt->ssid, sizeof(evt->ssid));
            memcpy(password, evt->password, sizeof(evt->password));
            printf("SSID:%s\n", ssid);
            printf("PASSWORD:%s\n", password);
            esp_wifi_disconnect();
            esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
            esp_wifi_connect();
            break;
        case SC_EVENT_SEND_ACK_DONE:
            break;

        default:
            break;
        }
    }
}
void wifi_init_func()
{
    // pharse 1
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_init = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init);

    // pharse 2
    esp_wifi_set_mode(WIFI_MODE_STA);
    const uint8_t protocol = WIFI_PROTOCOL_11B;
    esp_wifi_set_protocol(WIFI_IF_STA, protocol);

    // pharse 3
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL);
    esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "Metisai",
            .password = "zxcvbnm123"

        }};
    esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration);
    esp_wifi_start();
    esp_wifi_connect();
}
static void smartConfig_handler(void *para)
{
    // esptouch using app on martphone
    esp_smartconfig_set_type(SC_TYPE_ESPTOUCH);
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
    vTaskDelete(NULL);
}
void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    wifi_init_func();
    vTaskDelay(100);
}
