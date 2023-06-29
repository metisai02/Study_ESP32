#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"

TaskHandle_t task1;
const uart_port_t uart_num = UART_NUM_1;
void uart_config();
static void uartTask(void *para)
{
    char data[] = "helllllllooooo\n";
    while (1)
    {
      //  printf("hello\n");
        uart_write_bytes(uart_num, (void *)data, sizeof(data));
        vTaskDelay(100);
    }
}
void app_main(void)
{
    uint32_t sclk_freq;
    uart_config();
    uart_get_sclk_freq(UART_SCLK_APB, &sclk_freq);
    printf("UART clock frequency: %lu Hz\n", sclk_freq);
    xTaskCreate(uartTask, "uart", 1024, NULL, 10, &task1);

    gpio_isr_handler_add
}
void uart_config(void)
{
    uart_config_t uart2;
    uart_driver_install(uart_num, 1024, 0, 0, NULL, 0);
    uart2.baud_rate = 9600;
    uart2.data_bits = UART_DATA_8_BITS;
    uart2.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart2.parity = UART_PARITY_DISABLE;
    uart2.stop_bits = UART_STOP_BITS_1;
    uart2.rx_flow_ctrl_thresh = 0;
    uart2.source_clk = UART_SCLK_DEFAULT;
    uart_param_config(uart_num, &uart2);
}