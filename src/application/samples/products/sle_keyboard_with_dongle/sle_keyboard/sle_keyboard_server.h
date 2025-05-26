/**
 * @file sle_keyboard_server.h
 * @brief SLE keyboard server configuration header file / SLE键盘服务器配置头文件
 * @author @CompanyNameTag
 * @date 2023-07-29
 * @version 1.0
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 */

#ifndef SLE_KEYBOARD_SERVER_H
#define SLE_KEYBOARD_SERVER_H

#include <stdint.h>
#include "errcode.h"
#include "osal_debug.h"
#include "sle_ssap_server.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/** @brief Service UUID / 服务UUID */
#define SLE_UUID_SERVER_SERVICE 0x2222

/** @brief Property UUID for notification report / 通知报告属性UUID */
#define SLE_UUID_SERVER_NTF_REPORT 0x2323

/** @brief Property permissions (read and write) / 属性权限（读和写） */
#define SLE_UUID_TEST_PROPERTIES (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

/** @brief Operation indication flags / 操作指示标志 */
#define SLE_UUID_TEST_OPERATION_INDICATION (SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE)

/** @brief Descriptor permissions (read and write) / 描述符权限（读和写） */
#define SLE_UUID_TEST_DESCRIPTOR (SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)

/** @brief Print macro for debug output / 调试输出打印宏 */
#define sample_print(fmt, args...) osal_printk(fmt, ##args)
/** @brief Log tag for SLE keyboard server / SLE键盘服务器日志标签 */
#define SLE_KEYBOARD_SERVER_LOG "[sle keyboard server]"
/** @brief Server initialization delay in milliseconds / 服务器初始化延迟毫秒数 */
#define SLE_SERVER_INIT_DELAY_MS 1000

/**
 * @brief Initialize SLE keyboard server / 初始化SLE键盘服务器
 * @param[in] ssaps_read_callback Read request callback function / 读取请求回调函数
 * @param[in] ssaps_write_callback Write request callback function / 写入请求回调函数
 * @return Error code indicating success or failure / 表示成功或失败的错误码
 * @retval ERRCODE_SLE_SUCCESS Success / 成功
 * @retval Other error codes Failure / 其他错误码表示失败
 */
errcode_t sle_keyboard_server_init(ssaps_read_request_callback ssaps_read_callback,
                                   ssaps_write_request_callback ssaps_write_callback);

/**
 * @brief Send report by UUID / 通过UUID发送报告
 * @param[in] data Pointer to data to send / 要发送数据的指针
 * @param[in] len Length of data / 数据长度
 * @return Error code indicating success or failure / 表示成功或失败的错误码
 * @retval ERRCODE_SLE_SUCCESS Success / 成功
 * @retval ERRCODE_SLE_FAIL Failure / 失败
 */
errcode_t sle_keyboard_server_send_report_by_uuid(const uint8_t *data, uint8_t len);

/**
 * @brief Send report by handle / 通过句柄发送报告
 * @param[in] data Pointer to data to send / 要发送数据的指针
 * @param[in] len Length of data / 数据长度
 * @return Error code indicating success or failure / 表示成功或失败的错误码
 * @retval ERRCODE_SLE_SUCCESS Success / 成功
 * @retval ERRCODE_SLE_FAIL Failure / 失败
 */
errcode_t sle_keyboard_server_send_report_by_handle(const uint8_t *data, uint8_t len);

/**
 * @brief Check if keyboard client is connected / 检查键盘客户端是否已连接
 * @return Connection status / 连接状态
 * @retval 0 Not connected / 未连接
 * @retval 1 Connected / 已连接
 */
uint16_t sle_keyboard_client_is_connected(void);

/**
 * @brief Message queue callback function type / 消息队列回调函数类型
 * @param[in] buffer_addr Buffer address / 缓冲区地址
 * @param[in] buffer_size Buffer size / 缓冲区大小
 */
typedef void (*sle_keyboard_server_msg_queue)(uint8_t *buffer_addr, uint32_t buffer_size);

/**
 * @brief Register message queue for SLE keyboard server / 为SLE键盘服务器注册消息队列
 * @param[in] sle_keyboard_server_msg Message queue callback function / 消息队列回调函数
 */
void sle_keyboard_server_register_msg(sle_keyboard_server_msg_queue sle_keyboard_server_msg);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif