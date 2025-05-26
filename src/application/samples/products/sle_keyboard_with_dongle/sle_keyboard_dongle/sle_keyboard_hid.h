/**
 * @file sle_keyboard_hid.h
 * @brief SLE keyboard HID configuration header file / SLE键盘HID配置头文件
 * @author @CompanyNameTag
 * @date 2023-07-28
 * @version 1.0
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 */

#ifndef SLE_KEYBOARD_HID_H
#define SLE_KEYBOARD_HID_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/** @brief Maximum key length for USB HID keyboard / USB HID键盘最大按键长度 */
#define USB_HID_KEYBOARD_MAX_KEY_LENTH 6
/** @brief SLE keyboard dongle operation success / SLE键盘适配器操作成功 */
#define SLE_KEYBOARD_DONGLE_OK 0
/** @brief SLE keyboard dongle operation failed / SLE键盘适配器操作失败 */
#define SLE_KEYBOARD_DONGLE_FAILED 1

/**
 * @brief Definition of USB HID keyboard report structure / USB HID键盘报告结构定义
 */
typedef struct usb_hid_keyboard_report {
    uint8_t kind;        /**< Report kind / 报告类型 */
    uint8_t special_key; /**< 8-bit special key (Lctrl Lshift Lalt Lgui Rctrl Rshift Ralt Rgui) / 8位特殊键（左Ctrl
                            左Shift 左Alt 左Gui 右Ctrl 右Shift 右Alt 右Gui） */
    uint8_t reversed;    /**< Reserved field, must be zero / 保留字段，必须为零 */
    uint8_t key[USB_HID_KEYBOARD_MAX_KEY_LENTH]; /**< Normal key array / 普通按键数组 */
} usb_hid_keyboard_report_t;

/**
 * @brief Set HID report descriptor for SLE keyboard dongle / 为SLE键盘适配器设置HID报告描述符
 * @return Report descriptor index / 报告描述符索引
 * @retval >=0 Success, returns descriptor index / 成功，返回描述符索引
 * @retval <0 Failure / 失败
 */
int32_t sle_keyboard_dongle_set_report_desc_hid(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif