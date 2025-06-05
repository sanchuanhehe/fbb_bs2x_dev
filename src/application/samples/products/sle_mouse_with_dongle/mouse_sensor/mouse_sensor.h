/**
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * @file mouse_sensor.h
 * @brief Mouse sensor header
 * @author @CompanyNameTag
 * @date 2023-08-01
 */

#ifndef MOUSE_SENSOR_H
#define MOUSE_SENSOR_H

#include "stdint.h"
#include "stdbool.h"
#include "soc_osal.h"
#include "interrupt.h"
#include "../sle_mouse_server/sle_mouse_server.h"
#include "sle_connection_manager.h"
#include "sle_errcode.h"
#include "mouse_sensor_port.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @enum mouse_sensor
 * @brief 鼠标传感器类型
 */
typedef enum mouse_sensor {
    PWM3395DM,              /**< PAW3395传感器 */
    MOUSE_SENSOR_MAX_NUM    /**< 传感器数量上限 */
} mouse_sensor_t;

/**
 * @enum mouse_freq
 * @brief 鼠标采样频率
 */
typedef enum mouse_freq {
    MOUSE_FREQ_500 = 500,   /**< 500Hz */
    MOUSE_FREQ_1K  = 1000,  /**< 1KHz */
    MOUSE_FREQ_2K  = 2000,  /**< 2KHz */
    MOUSE_FREQ_4K  = 4000,  /**< 4KHz */
    MOUSE_FREQ_8K  = 8000,  /**< 8KHz */
    MOUSE_FREQ_MAX
} mouse_freq_t;

/**
 * @struct mouse_sensor_oprator
 * @brief 鼠标传感器操作结构体
 */
typedef struct mouse_sensor_oprator {
    mouse_freq_t (*init)(void);                /**< 初始化鼠标传感器 */
    void (*get_xy)(int16_t *x, int16_t *y);    /**< 获取鼠标坐标 */
} mouse_sensor_oprator_t;

#define DELAY_MS (1000)
#define SLE_MOUSE_TASK_DELAY_20_MS      20
#define SLE_MOUSE_TASK_DELAY_1700_MS    1700

#define trans_to_16_bit(num, bit) \
    ((((num) & (1 << ((bit) - 1))) != 0) ? ((num) | (0xFFFF - (1 << (bit)) + 1)) : (num))

/**
 * @brief 获取PAW3395操作结构体
 * @return mouse_sensor_oprator_t 操作结构体
 */
mouse_sensor_oprator_t sle_mouse_get_paw3395_operator(void);

/**
 * @brief 获取PAW3805操作结构体
 * @return mouse_sensor_oprator_t 操作结构体
 */
mouse_sensor_oprator_t sle_mouse_get_paw3805_operator(void);

/**
 * @brief 获取PMW3816操作结构体
 * @return mouse_sensor_oprator_t 操作结构体
 */
mouse_sensor_oprator_t sle_mouse_get_pmw3816_operator(void);

/**
 * @brief 鼠标SPI突发读取
 * @param reg_addr 起始寄存器地址
 * @param buf 数据缓冲区
 * @param lenth 读取长度
 */
void mouse_spi_burst_read(uint8_t reg_addr, uint8_t *buf, uint8_t lenth);

/**
 * @brief 写鼠标SPI寄存器
 * @param reg_addr 寄存器地址
 * @param val 写入的值
 */
void mouse_spi_write_reg(uint8_t reg_addr, uint8_t val);

/**
 * @brief 读取鼠标SPI寄存器
 * @param reg_addr 寄存器地址
 * @return uint8_t 读取到的寄存器值
 */
uint8_t mouse_spi_read_reg(uint8_t reg_addr);

/**
 * @brief 获取指定类型鼠标传感器的操作结构体
 * @param mouse_sensor 鼠标传感器类型
 * @return mouse_sensor_oprator_t 操作结构体
 */
mouse_sensor_oprator_t get_mouse_sensor_operator(mouse_sensor_t mouse_sensor);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif