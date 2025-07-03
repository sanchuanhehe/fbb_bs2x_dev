/**
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * @file mouse_wheel.h
 * @brief Mouse wheel header
 * @author @CompanyNameTag
 * @date 2023-08-01
 */

#ifndef MOUSE_WHEEL_H
#define MOUSE_WHEEL_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define QDEC_A CONFIG_MOUSE_PIN_QDEC_A
#define QDEC_B CONFIG_MOUSE_PIN_QDEC_B

/**
 * @brief 初始化鼠标滚轮
 * @param wheel 滚轮数据指针
 */
void mouse_wheel_init(int8_t *wheel);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif