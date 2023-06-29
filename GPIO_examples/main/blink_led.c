#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"

static const char *TAG = "led";

void config_gpio(void)
{

    gpio_config_t led;
    led.mode = GPIO_MODE_OUTPUT;
    led.pin_bit_mask = (1 << 2);
    led.pull_up_en = GPIO_PULLUP_ENABLE;
    led.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&led);
}
void app_main(void)
{
    config_gpio();
    // ESP_EARLY_LOGI(TAG, "hi");
    //printf("holle\n");
    while (1)
    {
        gpio_set_level(GPIO_NUM_2, 1);
        vTaskDelay(1000);
        gpio_set_level(GPIO_NUM_2, 0);
        vTaskDelay(1000);
    }
}
