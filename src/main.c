#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/uart.h>
#include "radio_bitfield_control.h"
#include "esp_log.h"

void app_main() {
    uart_bitfield_init(&rx_radio);
    uart_bitfield_init(&tx_radio);

    xTaskCreate(uart_bitfield_task, "rx_uart_tx_task", 2048, &rx_radio, 10, NULL);
    xTaskCreate(uart_bitfield_task, "tx_uart_tx_task", 2048, &tx_radio, 10, NULL);

    // ESP_LOGI(TAG, "UART bitfield tasks started at 100Hz");

    set_rx_mute(true);
    set_tx_restart(true);

    vTaskDelay(pdMS_TO_TICKS(3000));
    set_rx_mute(false);
    set_tx_restart(false);
}