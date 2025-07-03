/**
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * @file sle_mouse_client.h
 * @brief SLE Mouse Client header
 * @author @CompanyNameTag
 * @date 2023-08-01
 */
#ifndef SLE_MOUSE_CLIENT_H
#define SLE_MOUSE_CLIENT_H

#include "sle_ssap_client.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @brief SLE鼠标客户端初始化
 */
void sle_mouse_client_init(void);

/**
 * @brief 获取SLE鼠标客户端连接ID
 * @return 当前连接ID
 */
uint16_t get_g_sle_mouse_client_conn_id(void);

/**
 * @brief 获取SLE鼠标客户端连接状态
 * @return 当前连接状态
 */
uint8_t get_g_sle_mouse_client_conn_state(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif