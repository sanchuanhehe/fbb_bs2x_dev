/**
 * @file sle_keyboard_server_adv.h
 * @brief SLE ADV configuration header file / SLE广播配置头文件
 * @author @CompanyNameTag
 * @date 2023-07-29
 * @version 1.0
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 */

#ifndef SLE_SERVER_ADV_H
#define SLE_SERVER_ADV_H

/**
 * @brief Definition of SLE ADV common broadcast structure / SLE广播普通数据结构定义
 */
typedef struct sle_adv_common_value {
    uint8_t type;   /**< Data type / 数据类型 */
    uint8_t length; /**< Data length / 数据长度 */
    uint8_t value;  /**< Data value / 数据值 */
} sle_adv_common_value_t;

/**
 * @brief Definition of SLE ADV channel mapping / SLE广播信道映射定义
 */
typedef enum sle_adv_channel_map {
    SLE_ADV_CHANNEL_MAP_77 = 0x01,      /**< Channel 77 / 信道77 */
    SLE_ADV_CHANNEL_MAP_78 = 0x02,      /**< Channel 78 / 信道78 */
    SLE_ADV_CHANNEL_MAP_79 = 0x04,      /**< Channel 79 / 信道79 */
    SLE_ADV_CHANNEL_MAP_DEFAULT = 0x07  /**< Default channel map / 默认信道映射 */
} sle_adv_channel_map_t;

/**
 * @brief Definition of SLE ADV data type / SLE广播数据类型定义
 */
typedef enum sle_adv_data {
    SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL = 0x01,                         /**< Discovery level / 发现等级 */
    SLE_ADV_DATA_TYPE_ACCESS_MODE = 0x02,                             /**< Access layer capability / 接入层能力 */
    SLE_ADV_DATA_TYPE_SERVICE_DATA_16BIT_UUID = 0x03,                 /**< Standard service data information / 标准服务数据信息 */
    SLE_ADV_DATA_TYPE_SERVICE_DATA_128BIT_UUID = 0x04,                /**< Custom service data information / 自定义服务数据信息 */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_16BIT_SERVICE_UUIDS = 0x05,    /**< Complete list of 16-bit service UUIDs / 完整标准服务标识列表 */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_128BIT_SERVICE_UUIDS = 0x06,   /**< Complete list of 128-bit service UUIDs / 完整自定义服务标识列表 */
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_16BIT_SERVICE_UUIDS = 0x07,  /**< Incomplete list of 16-bit service UUIDs / 部分标准服务标识列表 */
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_128BIT_SERVICE_UUIDS = 0x08, /**< Incomplete list of 128-bit service UUIDs / 部分自定义服务标识列表 */
    SLE_ADV_DATA_TYPE_SERVICE_STRUCTURE_HASH_VALUE = 0x09,            /**< Service structure hash value / 服务结构散列值 */
    SLE_ADV_DATA_TYPE_SHORTENED_LOCAL_NAME = 0x0A,                    /**< Shortened local device name / 设备缩写本地名称 */
    SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME = 0x0B,                     /**< Complete local device name / 设备完整本地名称 */
    SLE_ADV_DATA_TYPE_TX_POWER_LEVEL = 0x0C,                          /**< TX power level / 广播发送功率 */
    SLE_ADV_DATA_TYPE_SLB_COMMUNICATION_DOMAIN = 0x0D,                /**< SLB communication domain name / SLB通信域域名 */
    SLE_ADV_DATA_TYPE_SLB_MEDIA_ACCESS_LAYER_ID = 0x0E,               /**< SLB media access layer ID / SLB媒体接入层标识 */
    SLE_ADV_DATA_TYPE_EXTENDED = 0xFE,                                /**< Extended data type / 数据类型扩展 */
    SLE_ADV_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA = 0xFF               /**< Manufacturer specific data / 厂商自定义信息 */
} sle_adv_data_t;

/**
 * @brief Initialize SLE keyboard server advertisement / 初始化SLE键盘服务器广播
 * @return Error code indicating success or failure / 表示成功或失败的错误码
 * @retval ERRCODE_SLE_SUCCESS Success / 成功
 * @retval Other error codes Failure / 其他错误码表示失败
 */
errcode_t sle_keyboard_server_adv_init(void);

/**
 * @brief Register callbacks for SLE keyboard announcement / 注册SLE键盘公告回调函数
 * @return Error code indicating success or failure / 表示成功或失败的错误码
 * @retval ERRCODE_SLE_SUCCESS Success / 成功
 * @retval Other error codes Failure / 其他错误码表示失败
 */
errcode_t sle_keyboard_announce_register_cbks(void);

#endif