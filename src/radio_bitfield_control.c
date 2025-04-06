#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "pinout.h"

static const char *TAG = "UART_BITFIELD";

typedef struct {
    uart_port_t uart_num;
    int tx_pin;
    int rx_pin;
    volatile uint8_t bitfield;
    portMUX_TYPE bitfield_lock;
    const char *name;
} uart_bitfield_device_t;

uart_bitfield_device_t rx_radio = {
    .uart_num = RX_RADIO_UART_NUM,
    .tx_pin = RX_RADIO_UART_TX,
    .rx_pin = RX_RADIO_UART_RX,
    .bitfield = 0,
    .bitfield_lock = portMUX_INITIALIZER_UNLOCKED,
    .name = "RX_RADIO"
};

uart_bitfield_device_t tx_radio = {
    .uart_num = TX_RADIO_UART_NUM,
    .tx_pin = TX_RADIO_UART_TX,
    .rx_pin = TX_RADIO_UART_RX,
    .bitfield = 0,
    .bitfield_lock = portMUX_INITIALIZER_UNLOCKED,
    .name = "TX_RADIO"
};

void uart_bitfield_init(uart_bitfield_device_t *dev) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(dev->uart_num, &uart_config);
    uart_set_pin(dev->uart_num, dev->tx_pin, dev->rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(dev->uart_num, 1024, 0, 0, NULL, 0);
}

void uart_bitfield_task(void *arg) {
    uart_bitfield_device_t *dev = (uart_bitfield_device_t *)arg;
    const TickType_t delay_ticks = pdMS_TO_TICKS(10);
    while (1) {
        uint8_t data;
        taskENTER_CRITICAL(&dev->bitfield_lock);
        data = dev->bitfield;
        taskEXIT_CRITICAL(&dev->bitfield_lock);

        uart_write_bytes(dev->uart_num, (const char*)&data, 1);
        vTaskDelay(delay_ticks);
    }
}

void set_field_state(uart_bitfield_device_t *dev, int bit_index, bool value) {
    taskENTER_CRITICAL(&dev->bitfield_lock);
    if (value)
        dev->bitfield |= (1 << bit_index);
    else
        dev->bitfield &= ~(1 << bit_index);
    taskEXIT_CRITICAL(&dev->bitfield_lock);
}

bool get_field_state(uart_bitfield_device_t *dev, int bit_index) {
    taskENTER_CRITICAL(&dev->bitfield_lock);
    bool state = (dev->bitfield >> bit_index) & 0x01;
    taskEXIT_CRITICAL(&dev->bitfield_lock);
    return state;
}

// RX Radio 字段访问函数
void set_rx_mute(bool val)            { set_field_state(&rx_radio, 0, val); }
bool get_rx_mute()                    { return get_field_state(&rx_radio, 0); }
void set_rx_transmitting(bool val)   { set_field_state(&rx_radio, 1, val); }
bool get_rx_transmitting()           { return get_field_state(&rx_radio, 1); }
void set_rx_restart(bool val)        { set_field_state(&rx_radio, 6, val); }
bool get_rx_restart()                { return get_field_state(&rx_radio, 6); }
void set_rx_hard_restart(bool val)   { set_field_state(&rx_radio, 7, val); }
bool get_rx_hard_restart()           { return get_field_state(&rx_radio, 7); }

// TX Radio 字段访问函数
void set_tx_mute(bool val)            { set_field_state(&tx_radio, 0, val); }
bool get_tx_mute()                    { return get_field_state(&tx_radio, 0); }
void set_tx_transmitting(bool val)   { set_field_state(&tx_radio, 1, val); }
bool get_tx_transmitting()           { return get_field_state(&tx_radio, 1); }
void set_tx_restart(bool val)        { set_field_state(&tx_radio, 6, val); }
bool get_tx_restart()                { return get_field_state(&tx_radio, 6); }
void set_tx_hard_restart(bool val)   { set_field_state(&tx_radio, 7, val); }
bool get_tx_hard_restart()           { return get_field_state(&tx_radio, 7); }
