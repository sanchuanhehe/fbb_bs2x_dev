/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse wheel header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
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
void mouse_wheel_init(int8_t *wheel);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif