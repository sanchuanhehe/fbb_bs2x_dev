/**
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * @file mouse_sensor.c
 * @brief Mouse sensor source
 * @author @CompanyNameTag
 * @date 2023-08-01
 */

#include "mouse_sensor.h"

/**
 * @brief 获取鼠标操作结构体的函数指针类型
 */
typedef mouse_sensor_oprator_t (*get_mouse_operator)(void);

/**
 * @brief 鼠标传感器操作函数表
 */
get_mouse_operator g_get_mouse_sensor_operators[] = {
#ifdef CONFIG_SAMPLE_SUPPORT_SENSOR_3395
    sle_mouse_get_paw3395_operator
#endif
#ifdef CONFIG_SAMPLE_SUPPORT_SENSOR_3805
    sle_mouse_get_paw3805_operator
#endif
#ifdef CONFIG_SAMPLE_SUPPORT_SENSOR_3816
    sle_mouse_get_pmw3816_operator
#endif
};

/**
 * @brief 获取指定类型鼠标传感器的操作结构体
 * @param mouse_sensor 鼠标传感器类型
 * @return mouse_sensor_oprator_t 操作结构体
 */
mouse_sensor_oprator_t get_mouse_sensor_operator(mouse_sensor_t mouse_sensor)
{
    get_mouse_operator operator_func = g_get_mouse_sensor_operators[mouse_sensor];
    return operator_func();
}
