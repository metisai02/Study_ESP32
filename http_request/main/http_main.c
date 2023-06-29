#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "esp_http_client.h"
#include "esp_system.h"
TaskHandle_t task1;

esp_http_client_handle_t client;
static void http_handler(void *para);
esp_err_t
_http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        printf("HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        printf("HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        printf("HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        printf("HTTP_EVENT_ON_HEADER, key=%s, value=%s\n", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        printf("HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        printf("HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        printf("HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}
static void
event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {

    case WIFI_EVENT_STA_START:
        printf("WIFI_EVENT_STA_START\n");
        /* code */
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WIFI_EVENT_STA_CONNECTED\n");
        /* code */
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
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
void wifi_http_client()
{
    esp_http_client_config_t config = {
        .url = "http://api.thingspeak.com/update.json",
        .host = "api.thingspeak.com",
        .method = HTTP_METHOD_POST,
        .event_handler = _http_event_handler};
    client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Connection", "close");
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
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
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = "Metisai",
            .password = "zxcvbnm123"

        }};
    esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration);
    esp_wifi_start();
    esp_wifi_connect();
}
static void http_handler(void *para)
{
    char post_data[100];
    int field1_value = 50;
    while (1)
    {

        snprintf(post_data, sizeof(post_data), "api_key=VYBPSDB5FDRKKKS0&field1=%d", field1_value);
        esp_http_client_set_post_field(client, post_data, strlen(post_data));

        // Thực hiện yêu cầu HTTP
        esp_http_client_perform(client);

        // Giải phóng tài nguyên
        // esp_http_client_cleanup(client);
        vTaskDelay(1000);
    }
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
    wifi_http_client();
    xTaskCreate(http_handler, "uart", 1024*3, NULL, 5, &task1);
}
