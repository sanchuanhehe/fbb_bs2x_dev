/**
 * @file sle_keyboard_hid.c
 * @brief SLE keyboard dongle HID source implementation / SLE键盘适配器HID源码实现
 * @author @CompanyNameTag
 * @date 2023-07-28
 * @version 1.0
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 */
#include "gadget/f_hid.h"

/** @brief HID input item with size / 带大小的HID输入项 */
#define input(size) (0x80 | (size))
/** @brief HID output item with size / 带大小的HID输出项 */
#define output(size) (0x90 | (size))
/** @brief HID feature item with size / 带大小的HID特性项 */
#define feature(size) (0xb0 | (size))
/** @brief HID collection item with size / 带大小的HID集合项 */
#define collection(size) (0xa0 | (size))
/** @brief HID end collection item with size / 带大小的HID结束集合项 */
#define end_collection(size) (0xc0 | (size))

/** @brief HID usage page global item / HID用法页全局项 */
#define usage_page(size) (0x04 | (size))
/** @brief HID logical minimum global item / HID逻辑最小值全局项 */
#define logical_minimum(size) (0x14 | (size))
/** @brief HID logical maximum global item / HID逻辑最大值全局项 */
#define logical_maximum(size) (0x24 | (size))
/** @brief HID physical minimum global item / HID物理最小值全局项 */
#define physical_minimum(size) (0x34 | (size))
/** @brief HID physical maximum global item / HID物理最大值全局项 */
#define physical_maximum(size) (0x44 | (size))
/** @brief HID unit exponent global item / HID单位指数全局项 */
#define uint_exponent(size) (0x54 | (size))
/** @brief HID unit global item / HID单位全局项 */
#define uint(size) (0x64 | (size))
/** @brief HID report size global item / HID报告大小全局项 */
#define report_size(size) (0x74 | (size))
/** @brief HID report ID global item / HID报告ID全局项 */
#define report_id(size) (0x84 | (size))
/** @brief HID report count global item / HID报告计数全局项 */
#define report_count(size) (0x94 | (size))
/** @brief HID push global item / HID推入全局项 */
#define push(size) (0xa4 | (size))
/** @brief HID pop global item / HID弹出全局项 */
#define pop(size) (0xb4 | (size))

/** @brief HID usage local item / HID用法本地项 */
#define usage(size) (0x08 | (size))
/** @brief HID usage minimum local item / HID用法最小值本地项 */
#define usage_minimum(size) (0x18 | (size))
/** @brief HID usage maximum local item / HID用法最大值本地项 */
#define usage_maximum(size) (0x28 | (size))
/** @brief HID designator index local item / HID指示器索引本地项 */
#define designator_index(size) (0x38 | (size))
/** @brief HID designator minimum local item / HID指示器最小值本地项 */
#define designator_minimum(size) (0x48 | (size))
/** @brief HID designator maximum local item / HID指示器最大值本地项 */
#define designator_maximum(size) (0x58 | (size))
/** @brief HID string index local item / HID字符串索引本地项 */
#define string_index(size) (0x78 | (size))
/** @brief HID string minimum local item / HID字符串最小值本地项 */
#define string_minimum(size) (0x88 | (size))
/** @brief HID string maximum local item / HID字符串最大值本地项 */
#define string_maximum(size) (0x98 | (size))
/** @brief HID delimiter local item / HID分隔符本地项 */
#define delimiter(size) (0xa8 | (size))

/** @brief HID report descriptor for keyboard and mouse / 键盘和鼠标HID报告描述符 */
static uint8_t g_report_desc_hid[] = {
    usage_page(1),
    0x01,
    usage(1),
    0x06,
    collection(1),
    0x01,
    report_id(1),
    0x01,

    usage_page(1),
    0x07,
    usage_minimum(1),
    0xE0,
    usage_maximum(1),
    0xE7,
    logical_minimum(1),
    0x00,
    logical_maximum(1),
    0x01,
    report_size(1),
    0x01,
    report_count(1),
    0x08,
    input(1),
    0x02,

    report_count(1),
    0x01,
    report_size(1),
    0x08,
    input(1),
    0x01,

    report_count(1),
    0x05,
    report_size(1),
    0x01,
    usage_page(1),
    0x08,
    usage_minimum(1),
    0x01,
    usage_maximum(1),
    0x05,
    output(1),
    0x02,
    report_count(1),
    0x01,
    report_size(1),
    0x03,
    output(1),
    0x01,

    report_count(1),
    0x06,
    report_size(1),
    0x08,
    logical_minimum(1),
    0x00,
    logical_maximum(1),
    0x65,
    usage_page(1),
    0x07,
    usage_minimum(1),
    0x00,
    usage_maximum(1),
    0x65,
    input(1),
    0x00,

    end_collection(0),

    usage_page(1),
    0x01,
    usage(1),
    0x02,
    collection(1),
    0x01,
    report_id(1),
    0x02,

    usage(1),
    0x01,
    collection(1),
    0x00,

    report_count(1),
    0x03,
    report_size(1),
    0x01,
    usage_page(1),
    0x09,
    usage_minimum(1),
    0x1,
    usage_maximum(1),
    0x3,
    logical_minimum(1),
    0x00,
    logical_maximum(1),
    0x01,
    input(1),
    0x02,
    report_count(1),
    0x01,
    report_size(1),
    0x05,
    input(1),
    0x01,
    report_count(1),
    0x03,
    report_size(1),
    0x08,
    usage_page(1),
    0x01,
    usage(1),
    0x30,
    usage(1),
    0x31,
    usage(1),
    0x38,
    logical_minimum(1),
    0x81,
    logical_maximum(1),
    0x7f,
    input(1),
    0x06,
    end_collection(0),
    end_collection(0),
    usage_page(2),
    0xB1,
    0xFF,
    usage(1),
    0x1,
    collection(1),
    0x01,
    report_id(1),
    0x08,
    collection(1),
    0x00,
    report_count(1),
    0xc,
    report_size(1),
    0x8,
    usage_minimum(1),
    0x0,
    usage_maximum(1),
    0xFF,
    logical_minimum(1),
    0,
    logical_maximum(1),
    0xff,
    output(1),
    2,
    end_collection(0),
    end_collection(0),
};

/**
 * @brief Set HID report descriptor for SLE keyboard dongle / 为SLE键盘适配器设置HID报告描述符
 * @return Report descriptor index / 报告描述符索引
 * @retval >=0 Success, returns descriptor index / 成功，返回描述符索引
 * @retval <0 Failure / 失败
 */
int32_t sle_keyboard_dongle_set_report_desc_hid(void)
{
    return hid_add_report_descriptor(g_report_desc_hid, sizeof(g_report_desc_hid), 0);
}