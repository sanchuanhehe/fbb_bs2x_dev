/**
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * @file sle_low_latency_service.h
 * @brief Sle Low Latency Mouse Header
 * @author @CompanyNameTag
 * @date 2023-08-01
 */

#ifndef SLE_LOW_LAYENCY_SERVICE_H
#define SLE_LOW_LAYENCY_SERVICE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief 低延迟鼠标应用初始化
 */
void sle_low_latency_mouse_app_init(void);

/**
 * @brief 低延迟Dongle初始化
 * @param usb_hid_index USB HID索引
 */
void sle_low_latency_dongle_init(int usb_hid_index);

/**
 * @brief 鼠标初始化
 * @param sensor_id 传感器ID
 * @return mouse_freq_t 鼠标频率
 */
mouse_freq_t mouse_init(uint32_t sensor_id);

/**
 * @brief 获取鼠标按键信息
 */
void sle_mouse_get_key(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif