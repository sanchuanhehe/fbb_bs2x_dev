/**
 * @copyright Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
 * @file mouse_wheel.c
 * @brief Mouse wheel source
 * @author @CompanyNameTag
 * @date 2023-08-01
 */

#include "qdec.h"
#include "common_def.h"
#include "mouse_wheel.h"

/**
 * @brief 鼠标滚轮全局指针
 */
static int8_t *g_wheel = NULL;

/**
 * @brief QDEC配置结构体
 */
static qdec_config_t g_qdec_config = QDEC_DEFAULT_CONFIG;

/**
 * @brief QDEC中断回调，处理滚轮数据
 * @param argc 滚轮增量
 * @param argv 未使用
 * @return int 0成功，1失败
 */
static int qdec_report_callback(int argc, char *argv[])
{
    unused(argv);

    if (g_wheel == NULL) {
        return 1;
    }
    *g_wheel = -argc;
    return 0;
}

/**
 * @brief 初始化鼠标滚轮
 * @param wheel 滚轮数据指针
 */
void mouse_wheel_init(int8_t *wheel)
{
    g_wheel = wheel;

    uapi_qdec_init(&g_qdec_config);
    qdec_port_pinmux_init(QDEC_A, QDEC_B);
    uapi_qdec_register_callback(qdec_report_callback);
    uapi_qdec_enable();
}