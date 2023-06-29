/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO 2
#define BUTTON_GPIO 0
//#define CONFIG_BLINK_PERIOD 100;
static uint8_t s_led_state = 0;

void blink_led();
void configure_gpio();

void app_main(void)
{

    /* Configure the peripheral according to the LED type */
    ESP_EARLY_LOGI(TAG, "hi");
    configure_gpio();

    while (1) {
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        
    }
}
void configure_gpio()
{
    gpio_config_t gpio;
    gpio.mode = GPIO_MODE_OUTPUT;
    gpio.pin_bit_mask = GPIO_NUM_2;
    gpio.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&gpio);
}
void blink_led()
{
    gpio_set_level(GPIO_NUM_2,s_led_state);
}