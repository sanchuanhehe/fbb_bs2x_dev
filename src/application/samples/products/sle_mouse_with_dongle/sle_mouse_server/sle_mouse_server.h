/**
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * @file sle_mouse_server.h
 * @brief SLE mouse server Header
 * @author @CompanyNameTag
 * @date 2023-08-01
 */

#ifndef SLE_MOUSE_SERVER_H
#define SLE_MOUSE_SERVER_H

#include "sle_ssap_server.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @struct sle_item_handle_t
 * @brief SLE服务句柄结构体
 */
typedef struct {
    uint16_t handle_in;   /**< 输入句柄 */
    uint16_t handle_out;  /**< 输出句柄 */
} sle_item_handle_t;

/**
 * @enum sle_handle_t
 * @brief SLE句柄类型
 */
typedef enum {
    HANDLE_IN,
    HANDLE_OUT,
    HANDLE_INVALID,
} sle_handle_t;

/**
 * @struct ssap_mouse_key_t
 * @brief SLE鼠标数据结构体
 */
typedef struct {
    int32_t button_mask : 8; /**< 按键掩码 */
    int32_t x : 12;          /**< mouse的x坐标 */
    int32_t y : 12;          /**< mouse的y坐标 */
    int8_t wheel;            /**< 滚轮 */
} ssap_mouse_key_t;

/**
 * @brief SLE鼠标服务器初始化
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 */
errcode_t sle_mouse_server_init(void);

/**
 * @brief SLE读取ssap通道能力
 * @param param 能力指针
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 */
errcode_t get_g_read_ssap_support(bool *param);

/**
 * @brief SLE读取连接状态
 * @param conn_state 连接状态指针
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 */
errcode_t get_g_sle_mouse_server_conn_state(uint8_t *conn_state);

/**
 * @brief SLE读取配对状态
 * @param pair_state 配对状态指针
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 */
errcode_t get_g_sle_mouse_pair_state(uint32_t *pair_state);

/**
 * @brief SLE发送鼠标数据
 * @param mouse_data 鼠标数据结构体指针
 * @retval ERRCODE_SLE_SUCCESS    执行成功
 * @retval ERRCODE_SLE_FAIL       执行失败
 */
errcode_t sle_hid_mouse_server_send_input_report(ssap_mouse_key_t *mouse_data);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif