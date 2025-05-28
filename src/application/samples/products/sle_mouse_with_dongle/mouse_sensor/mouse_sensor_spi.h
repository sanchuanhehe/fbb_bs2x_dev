/**
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * @file mouse_sensor_spi.h
 * @brief Mouse sensor spi
 * @author @CompanyNameTag
 * @date 2023-08-01
 */

#ifndef MOUSE_SENSOR_SPI_H
#define MOUSE_SENSOR_SPI_H

#include "stdint.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief 鼠标SPI传感器回调函数类型
 */
typedef void (*mouse_spi_sensor_func_t)(void);

#define HAL_SPI_ENABLE                   0x01
#define HAL_SPI_TRANS_MODE_MAX           0x03
#define HAL_SPI_TRANS_MODE_SHIFT         0x08
#define HAL_SPI_TRANS_MODE_TXRX          0x00
#define HAL_SPI_TRANS_MODE_TX            0x01
#define HAL_SPI_TRANS_MODE_RX            0x02
#define HAL_SPI_TRANS_MODE_EEPROM        0x03
#define HAL_SPI_RECEIVED_DATA_REG_MAX    0xFFFF
#define HAL_SPI_CE_LIN_TOGGLE_ENABLE     (BIT(24))
#define HAL_SPI_TX_FIFO_NOT_FULL_FLAG    (BIT(1))
#define HAL_SPI_RX_FIFO_NOT_EMPTY_FLAG   (BIT(3))
//#define MOUSE_SPI SPI_BUS_2 //todo 宏定义冲突，将此处注释掉了。mouse_sensor_port.h中同样定义了#define MOUSE_SPI SPI_BUS_0

/**
 * @enum mouse_opration
 * @brief 鼠标SPI操作类型
 */
typedef enum mouse_opration {
    READ,      /**< 读操作 */
    WRITE,     /**< 写操作 */
    DELAY,     /**< 延时操作 */
    RUN_FUNC,  /**< 执行函数 */
    MAX_OPRATION
} mouse_opration_t;

/**
 * @struct spi_mouse_cfg
 * @brief 鼠标SPI配置结构体
 */
typedef struct spi_mouse_cfg {
    mouse_opration_t opration;      /**< 操作类型 */
    uint16_t addr;                  /**< 寄存器地址或延时参数 */
    int16_t value;                  /**< 写入值或其他参数 */
    mouse_spi_sensor_func_t func;   /**< 回调函数指针 */
} spi_mouse_cfg_t;

/**
 * @brief 打开鼠标SPI接口
 * @param frame_format 帧格式
 * @param clk_polarity 时钟极性
 * @param clk_phase 时钟相位
 * @param mhz SPI时钟频率MHz
 */
void mouse_sensor_spi_open(uint8_t frame_format, uint8_t clk_polarity, uint8_t clk_phase, uint8_t mhz);

/**
 * @brief 按配置表操作鼠标SPI
 * @param cfg 配置表指针
 * @param lenth 配置表长度
 */
void mouse_sensor_spi_opration(const spi_mouse_cfg_t *cfg, int16_t lenth);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif