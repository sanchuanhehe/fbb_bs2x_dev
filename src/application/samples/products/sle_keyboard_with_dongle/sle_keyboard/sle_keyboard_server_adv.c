/**
 * @file sle_keyboard_server_adv.c
 * @brief SLE advertisement configuration for SLE keyboard server / SLE键盘服务器广播配置
 * @author @CompanyNameTag
 * @date 2023-07-29
 * @version 1.0
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 */
#include "securec.h"
#include "errcode.h"
#include "osal_addr.h"
#include "common_def.h"
#include "sle_common.h"
#include "sle_device_discovery.h"
#include "sle_errcode.h"
#include "sle_keyboard_server.h"
#include "sle_keyboard_server_adv.h"
#include "sle_device_manager.h"

/** @brief Maximum length of SLE device name / SLE设备名称最大长度 */
#define NAME_MAX_LENGTH 32
/** @brief Connection interval minimum 12.5ms, unit 125us / 连接调度间隔最小值12.5ms，单位125us */
#define SLE_CONN_INTV_MIN_DEFAULT 0x64
/** @brief Connection interval maximum 12.5ms, unit 125us / 连接调度间隔最大值12.5ms，单位125us */
#define SLE_CONN_INTV_MAX_DEFAULT 0x64
/** @brief Advertisement interval minimum 25ms, unit 125us / 广播间隔最小值25ms，单位125us */
#define SLE_ADV_INTERVAL_MIN_DEFAULT 0xC8
/** @brief Advertisement interval maximum 25ms, unit 125us / 广播间隔最大值25ms，单位125us */
#define SLE_ADV_INTERVAL_MAX_DEFAULT 0xC8
/** @brief Connection supervision timeout 5000ms, unit 10ms / 连接监管超时时间5000ms，单位10ms */
#define SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT 0x1F4
/** @brief Connection maximum latency 4990ms, unit 10ms / 连接最大延迟4990ms，单位10ms */
#define SLE_CONN_MAX_LATENCY 0x1F3
/** @brief Advertisement transmission power / 广播发送功率 */
#define SLE_ADV_TX_POWER 10
/** @brief Default advertisement handle ID / 默认广播句柄ID */
#define SLE_ADV_HANDLE_DEFAULT 1
/** @brief Maximum advertisement data length / 最大广播数据长度 */
#define SLE_ADV_DATA_LEN_MAX 251

/** @brief Local device name for SLE keyboard server / SLE键盘服务器本地设备名称 */
static uint8_t sle_local_name[] = "sle_keyboard_server";

/**
 * @brief Set advertisement local name data / 设置广播本地名称数据
 * @param[out] adv_data Pointer to advertisement data buffer / 广播数据缓冲区指针
 * @param[in] max_len Maximum length of advertisement data buffer / 广播数据缓冲区最大长度
 * @return Length of local name data added to advertisement / 添加到广播中的本地名称数据长度
 */
static uint16_t sle_set_adv_local_name(uint8_t *adv_data, uint16_t max_len)
{
    uint8_t index = 0;
    uint8_t *local_name = sle_local_name;
    uint8_t local_name_len = sizeof(sle_local_name) - 1;

    sample_print("%s local_name_len = %d\r\n", SLE_KEYBOARD_SERVER_LOG, local_name_len);
    sample_print("%s local_name: ", SLE_KEYBOARD_SERVER_LOG);
    for (uint8_t i = 0; i < local_name_len; i++) {
        sample_print("0x%02x ", local_name[i]);
    }
    sample_print("\r\n");
    adv_data[index++] = local_name_len + 1;
    adv_data[index++] = SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME;
    if (memcpy_s(&adv_data[index], max_len - index, local_name, local_name_len) != EOK) {
        sample_print("%s memcpy fail\r\n", SLE_KEYBOARD_SERVER_LOG);
        return 0;
    }
    return (uint16_t)index + local_name_len;
}

/**
 * @brief Set advertisement data / 设置广播数据
 * @param[out] adv_data Pointer to advertisement data buffer / 广播数据缓冲区指针
 * @param[in] length Length of advertisement data buffer / 广播数据缓冲区长度
 * @return Length of advertisement data set / 设置的广播数据长度
 */
static uint16_t sle_set_adv_data(uint8_t *adv_data, uint16_t length)
{
    size_t len = 0;
    uint16_t idx = 0;

    unused(length);
    len = sizeof(struct sle_adv_common_value);
    struct sle_adv_common_value adv_disc_level = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL,
        .value = SLE_ANNOUNCE_LEVEL_NORMAL,
    };
    if (memcpy_s(&adv_data[idx], SLE_ADV_DATA_LEN_MAX - idx, &adv_disc_level, len) != EOK) {
        sample_print("%s adv_disc_level memcpy fail\r\n", SLE_KEYBOARD_SERVER_LOG);
        return 0;
    }
    idx += len;

    len = sizeof(struct sle_adv_common_value);
    struct sle_adv_common_value adv_access_mode = {
        .length = len - 1,
        .type = SLE_ADV_DATA_TYPE_ACCESS_MODE,
        .value = 0,
    };
    if (memcpy_s(&adv_data[idx], SLE_ADV_DATA_LEN_MAX - idx, &adv_access_mode, len) != EOK) {
        sample_print("%s adv_access_mode memcpy fail\r\n", SLE_KEYBOARD_SERVER_LOG);
        return 0;
    }
    idx += len;

    return idx;
}

/**
 * @brief Set scan response data / 设置扫描响应数据
 * @param[out] scan_rsp_data Pointer to scan response data buffer / 扫描响应数据缓冲区指针
 * @param[in] length Length of scan response data buffer / 扫描响应数据缓冲区长度
 * @return Length of scan response data set / 设置的扫描响应数据长度
 */
static uint16_t sle_set_scan_response_data(uint8_t *scan_rsp_data, uint16_t length)
{
    uint16_t idx = 0;
    size_t scan_rsp_data_len = sizeof(struct sle_adv_common_value);

    unused(length);
    struct sle_adv_common_value tx_power_level = {
        .length = scan_rsp_data_len - 1,
        .type = SLE_ADV_DATA_TYPE_TX_POWER_LEVEL,
        .value = SLE_ADV_TX_POWER,
    };
    if (memcpy_s(scan_rsp_data, SLE_ADV_DATA_LEN_MAX, &tx_power_level, scan_rsp_data_len) != EOK) {
        sample_print("%s sle scan response data memcpy fail\r\n", SLE_KEYBOARD_SERVER_LOG);
        return 0;
    }
    idx += scan_rsp_data_len;

    /** Set local name / 设置本地名称 */
    idx += sle_set_adv_local_name(&scan_rsp_data[idx], SLE_ADV_DATA_LEN_MAX - idx);
    return idx;
}

/**
 * @brief Set default announcement parameters / 设置默认公告参数
 * @return Error code indicating success or failure / 表示成功或失败的错误码
 * @retval ERRCODE_SLE_SUCCESS Success / 成功
 * @retval Other error codes Failure / 其他错误码表示失败
 */
static int sle_set_default_announce_param(void)
{
    sle_announce_param_t param = {0};
    unsigned char local_addr[SLE_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    param.announce_mode = SLE_ANNOUNCE_MODE_CONNECTABLE_SCANABLE;
    param.announce_handle = SLE_ADV_HANDLE_DEFAULT;
    param.announce_gt_role = SLE_ANNOUNCE_ROLE_T_CAN_NEGO;
    param.announce_level = SLE_ANNOUNCE_LEVEL_NORMAL;
    param.announce_channel_map = SLE_ADV_CHANNEL_MAP_DEFAULT;
    param.announce_interval_min = SLE_ADV_INTERVAL_MIN_DEFAULT;
    param.announce_interval_max = SLE_ADV_INTERVAL_MAX_DEFAULT;
    param.conn_interval_min = SLE_CONN_INTV_MIN_DEFAULT;
    param.conn_interval_max = SLE_CONN_INTV_MAX_DEFAULT;
    param.conn_max_latency = SLE_CONN_MAX_LATENCY;
    param.conn_supervision_timeout = SLE_CONN_SUPERVISION_TIMEOUT_DEFAULT;
    param.own_addr.type = 0;
    if (memcpy_s(param.own_addr.addr, SLE_ADDR_LEN, local_addr, SLE_ADDR_LEN) != EOK) {
        sample_print("%s sle_set_default_announce_param data memcpy fail\r\n", SLE_KEYBOARD_SERVER_LOG);
        return 0;
    }
    sample_print("%s sle_keyboard_local addr: ", SLE_KEYBOARD_SERVER_LOG);
    for (uint8_t index = 0; index < SLE_ADDR_LEN; index++) {
        sample_print("0x%02x ", param.own_addr.addr[index]);
    }
    sample_print("\r\n");
    return sle_set_announce_param(param.announce_handle, &param);
}

/**
 * @brief Set default announcement data / 设置默认公告数据
 * @return Error code indicating success or failure / 表示成功或失败的错误码
 * @retval ERRCODE_SLE_SUCCESS Success / 成功
 * @retval Other error codes Failure / 其他错误码表示失败
 */
static int sle_set_default_announce_data(void)
{
    errcode_t ret;
    uint8_t announce_data_len = 0;
    uint8_t seek_data_len = 0;
    sle_announce_data_t data = {0};
    uint8_t adv_handle = SLE_ADV_HANDLE_DEFAULT;
    uint8_t announce_data[SLE_ADV_DATA_LEN_MAX] = {0};
    uint8_t seek_rsp_data[SLE_ADV_DATA_LEN_MAX] = {0};

    announce_data_len = sle_set_adv_data(announce_data, SLE_ADV_DATA_LEN_MAX);
    data.announce_data = announce_data;
    data.announce_data_len = announce_data_len;

    sample_print("%s data.announce_data_len = %d\r\n", SLE_KEYBOARD_SERVER_LOG, data.announce_data_len);
    sample_print("%s data.announce_data: ", SLE_KEYBOARD_SERVER_LOG);
    for (uint8_t data_index = 0; data_index < data.announce_data_len; data_index++) {
        sample_print("0x%02x ", data.announce_data[data_index]);
    }
    sample_print("\r\n");

    seek_data_len = sle_set_scan_response_data(seek_rsp_data, SLE_ADV_DATA_LEN_MAX);
    data.seek_rsp_data = seek_rsp_data;
    data.seek_rsp_data_len = seek_data_len;

    sample_print("%s data.seek_rsp_data_len = %d\r\n", SLE_KEYBOARD_SERVER_LOG, data.seek_rsp_data_len);
    sample_print("%s data.seek_rsp_data: ", SLE_KEYBOARD_SERVER_LOG);
    for (uint8_t data_index = 0; data_index < data.seek_rsp_data_len; data_index++) {
        sample_print("0x%02x ", data.seek_rsp_data[data_index]);
    }
    sample_print("\r\n");

    ret = sle_set_announce_data(adv_handle, &data);
    if (ret == ERRCODE_SLE_SUCCESS) {
        sample_print("%s set announce data success.\r\n", SLE_KEYBOARD_SERVER_LOG);
    } else {
        sample_print("%s set adv param fail.\r\n", SLE_KEYBOARD_SERVER_LOG);
    }
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @brief Callback function for announcement enable event / 公告启用事件回调函数
 * @param[in] announce_id Announcement ID / 公告ID
 * @param[in] status Status of the enable operation / 启用操作状态
 */
static void sle_announce_enable_cbk(uint32_t announce_id, errcode_t status)
{
    sample_print("%s sle announce enable callback id:%02x, state:%x\r\n", SLE_KEYBOARD_SERVER_LOG, announce_id, status);
}

/**
 * @brief Callback function for announcement disable event / 公告禁用事件回调函数
 * @param[in] announce_id Announcement ID / 公告ID
 * @param[in] status Status of the disable operation / 禁用操作状态
 */
static void sle_announce_disable_cbk(uint32_t announce_id, errcode_t status)
{
    sample_print("%s sle announce disable callback id:%02x, state:%x\r\n", SLE_KEYBOARD_SERVER_LOG, announce_id,
                 status);
}

/**
 * @brief Callback function for announcement terminal event / 公告终端事件回调函数
 * @param[in] announce_id Announcement ID / 公告ID
 */
static void sle_announce_terminal_cbk(uint32_t announce_id)
{
    sample_print("%s sle announce terminal callback id:%02x\r\n", SLE_KEYBOARD_SERVER_LOG, announce_id);
}

/**
 * @brief Callback function for SLE enable event / SLE启用事件回调函数
 * @param[in] status Status of the SLE enable operation / SLE启用操作状态
 */
static void sle_enable_cb(uint8_t status)
{
    sample_print("%s sle enable callback status:%x\r\n", SLE_KEYBOARD_SERVER_LOG, status);
}

/**
 * @brief Register callbacks for SLE keyboard announcement / 注册SLE键盘公告回调函数
 * @return Error code indicating success or failure / 表示成功或失败的错误码
 * @retval ERRCODE_SLE_SUCCESS Success / 成功
 * @retval Other error codes Failure / 其他错误码表示失败
 */
errcode_t sle_keyboard_announce_register_cbks(void)
{
    errcode_t ret;
    sle_dev_manager_callbacks_t dev_mgr_cbk = {0};
    sle_announce_seek_callbacks_t seek_cbks = {0};
    seek_cbks.announce_enable_cb = sle_announce_enable_cbk;
    seek_cbks.announce_disable_cb = sle_announce_disable_cbk;
    seek_cbks.announce_terminal_cb = sle_announce_terminal_cbk;
    dev_mgr_cbk.sle_enable_cb = sle_enable_cb;
    ret = sle_announce_seek_register_callbacks(&seek_cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_keyboard_announce_register_cbks,register_callbacks fail :%x\r\n", SLE_KEYBOARD_SERVER_LOG,
                     ret);
        return ret;
    }
    ret = sle_dev_manager_register_callbacks(&dev_mgr_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_keyboard_announce_register_cbks,register_callbacks fail :%x\r\n", SLE_KEYBOARD_SERVER_LOG,
                     ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @brief Initialize SLE keyboard server advertisement / 初始化SLE键盘服务器广播
 * @return Error code indicating success or failure / 表示成功或失败的错误码
 * @retval ERRCODE_SLE_SUCCESS Success / 成功
 * @retval Other error codes Failure / 其他错误码表示失败
 */
errcode_t sle_keyboard_server_adv_init(void)
{
    errcode_t ret;
    sle_set_default_announce_param();
    sle_set_default_announce_data();
    ret = sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    if (ret != ERRCODE_SLE_SUCCESS) {
        sample_print("%s sle_keyboard_server_adv_init,sle_start_announce fail :%x\r\n", SLE_KEYBOARD_SERVER_LOG, ret);
        return ret;
    }
    return ERRCODE_SLE_SUCCESS;
}
