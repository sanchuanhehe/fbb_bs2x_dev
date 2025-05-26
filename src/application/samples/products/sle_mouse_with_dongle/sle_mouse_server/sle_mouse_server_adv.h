/**
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * @file sle_mouse_server_adv.h
 * @brief Sle Mouse server adv header
 * @author @CompanyNameTag
 * @date 2023-08-01
 */

#ifndef SLE_MOUSE_SERVER_ADV_H
#define SLE_MOUSE_SERVER_ADV_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define SLE_MOUSE_DONGLE_SERVER_LOG                     "[sle mouse dongle server]"

/**
 * @struct sle_adv_common
 * @brief 广播通用结构体
 */
typedef struct sle_adv_common {
    uint8_t type;    /**< 类型 */
    uint8_t length;  /**< 长度 */
    uint8_t value;   /**< 值 */
} sle_adv_common_t;

/**
 * @enum sle_adv_channel_map_t
 * @brief 广播信道掩码
 */
typedef enum sle_adv_channel {
    SLE_ADV_CHANNEL_MAP_77                 = 0x01, /**< 信道77 */
    SLE_ADV_CHANNEL_MAP_78                 = 0x02, /**< 信道78 */
    SLE_ADV_CHANNEL_MAP_79                 = 0x04, /**< 信道79 */
    SLE_ADV_CHANNEL_MAP_DEFAULT            = 0x07  /**< 默认信道 */
} sle_adv_channel_map_t;

/**
 * @enum sle_adv_data_t
 * @brief 广播数据类型
 */
typedef enum sle_adv_data {
    SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL                              = 0x01,   /**< 发现等级 */
    SLE_ADV_DATA_TYPE_ACCESS_MODE                                  = 0x02,   /**< 接入层能力 */
    SLE_ADV_DATA_TYPE_SERVICE_DATA_16BIT_UUID                      = 0x03,   /**< 标准服务数据信息 */
    SLE_ADV_DATA_TYPE_SERVICE_DATA_128BIT_UUID                     = 0x04,   /**< 自定义服务数据信息 */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_16BIT_SERVICE_UUIDS         = 0x05,   /**< 完整标准服务标识列表 */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_128BIT_SERVICE_UUIDS        = 0x06,   /**< 完整自定义服务标识列表 */
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_16BIT_SERVICE_UUIDS       = 0x07,   /**< 部分标准服务标识列表 */
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_128BIT_SERVICE_UUIDS      = 0x08,   /**< 部分自定义服务标识列表 */
    SLE_ADV_DATA_TYPE_SERVICE_STRUCTURE_HASH_VALUE                 = 0x09,   /**< 服务结构散列值 */
    SLE_ADV_DATA_TYPE_SHORTENED_LOCAL_NAME                         = 0x0A,   /**< 设备缩写本地名称 */
    SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME                          = 0x0B,   /**< 设备完整本地名称 */
    SLE_ADV_DATA_TYPE_TX_POWER_LEVEL                               = 0x0C,   /**< 广播发送功率 */
    SLE_ADV_DATA_TYPE_SLB_COMMUNICATION_DOMAIN                     = 0x0D,   /**< SLB通信域域名 */
    SLE_ADV_DATA_TYPE_SLB_MEDIA_ACCESS_LAYER_ID                    = 0x0E,   /**< SLB媒体接入层标识 */
    SLE_ADV_DATA_TYPE_EXTENDED                                     = 0xFE,   /**< 数据类型扩展 */
    SLE_ADV_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA                   = 0xFF    /**< 厂商自定义信息 */
} sle_adv_data_t;

/**
 * @brief SLE鼠标服务端广播初始化
 */
void sle_mouse_server_adv_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
