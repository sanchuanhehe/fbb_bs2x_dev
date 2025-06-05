/**
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * @file usb_init_app.h
 * @brief USB Initialize Header
 * @author @CompanyNameTag
 * @date 2023-08-01
 */
#ifndef USB_INIT_APP_H
#define USB_INIT_APP_H

#include "implementation/usb_init.h"
#include "gadget/f_hid.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief USB初始化
 * @param dtype 设备类型
 * @return int HID索引，失败返回-1
 */
int usb_init_app(device_type dtype);

/**
 * @brief USB反初始化
 * @return int 总是返回0
 */
int usb_deinit_app(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif