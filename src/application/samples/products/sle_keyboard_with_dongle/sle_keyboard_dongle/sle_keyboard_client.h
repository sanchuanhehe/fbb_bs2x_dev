/**
 * @file sle_keyboard_client.h
 * @brief SLE keyboard client sample header file / SLE键盘客户端示例头文件
 * @author @CompanyNameTag
 * @date 2023-04-03
 * @version 1.0
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 */
#ifndef SLE_KEYBOARD_CLIENT_H
#define SLE_KEYBOARD_CLIENT_H

#include "sle_ssap_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief Initialize SLE keyboard client / 初始化SLE键盘客户端
 * @param[in] notification_cb Notification callback function / 通知回调函数
 * @param[in] indication_cb Indication callback function / 指示回调函数
 */
void sle_keyboard_client_init(ssapc_notification_callback notification_cb, ssapc_indication_callback indication_cb);

/**
 * @brief Start SLE keyboard scan / 启动SLE键盘扫描
 */
void sle_keyboard_start_scan(void);

/**
 * @brief Get SLE keyboard connection ID / 获取SLE键盘连接ID
 * @return Connection ID / 连接ID
 */
uint16_t get_sle_keyboard_conn_id(void);

/**
 * @brief Get SLE keyboard send parameters / 获取SLE键盘发送参数
 * @return Write parameters structure / 写入参数结构
 */
ssapc_write_param_t get_sle_keyboard_send_param(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif