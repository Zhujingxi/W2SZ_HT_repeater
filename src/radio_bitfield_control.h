#pragma once

#include "driver/uart.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uart_port_t uart_num;
    int tx_pin;
    int rx_pin;
    volatile uint8_t bitfield;
    portMUX_TYPE bitfield_lock;
    const char *name;
} uart_bitfield_device_t;

// 外部设备定义
extern uart_bitfield_device_t rx_radio;
extern uart_bitfield_device_t tx_radio;

// 初始化和任务函数
void uart_bitfield_init(uart_bitfield_device_t *dev);
void uart_bitfield_task(void *arg);

// 通用字段操作函数
void set_field_state(uart_bitfield_device_t *dev, int bit_index, bool value);
bool get_field_state(uart_bitfield_device_t *dev, int bit_index);

// RX 字段函数
void set_rx_mute(bool val);
bool get_rx_mute();
void set_rx_transmitting(bool val);
bool get_rx_transmitting();
void set_rx_restart(bool val);
bool get_rx_restart();
void set_rx_hard_restart(bool val);
bool get_rx_hard_restart();

// TX 字段函数
void set_tx_mute(bool val);
bool get_tx_mute();
void set_tx_transmitting(bool val);
bool get_tx_transmitting();
void set_tx_restart(bool val);
bool get_tx_restart();
void set_tx_hard_restart(bool val);
bool get_tx_hard_restart();

#ifdef __cplusplus
}
#endif
