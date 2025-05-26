/**
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * @file mouse_button.h
 * @brief Mouse Button header
 * @author @CompanyNameTag
 * @date 2023-08-01
 */

#ifndef MOUSE_BUTTON_H
#define MOUSE_BUTTON_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @union mouse_key
 * @brief 鼠标按键联合体，支持位域和字节访问
 */
typedef union mouse_key {
    /**
     * @struct mouse_key::b
     * @brief 鼠标按键位域结构
     */
    struct {
        uint8_t left_key   : 1; /**< 左键 */
        uint8_t right_key  : 1; /**< 右键 */
        uint8_t mid_key    : 1; /**< 中键 */
        uint8_t reserved   : 5; /**< 保留 */
    } b;
    uint8_t d8; /**< 按键状态字节访问 */
} mouse_key_t;

/**
 * @brief 初始化鼠标按键结构体
 * @param mouse_key [out] 鼠标按键结构体指针
 */
void mouse_button_init(mouse_key_t *mouse_key);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif