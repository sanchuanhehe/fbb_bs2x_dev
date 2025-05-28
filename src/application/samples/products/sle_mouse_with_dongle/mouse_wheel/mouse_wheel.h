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

#define QDEC_A S_MGPIO4
#define QDEC_B S_MGPIO3 // todo 两个QDEC的宏定义是直接copy的，可能会有冲突

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