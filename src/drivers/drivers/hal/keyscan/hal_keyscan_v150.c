/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides v150 hal keyscan \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include <stdint.h>
#include <stdio.h>
#include "securec.h"
#include "oal_interface.h"
#include "errcode.h"
#include "common_def.h"
#include "chip_core_irq.h"
#include "keyscan_porting.h"
#include "hal_keyscan.h"
#include "hal_keyscan_v150_regs_op.h"
#include "soc_osal.h"
#include "hal_keyscan_v150.h"

static hal_keyscan_callback_t g_hal_keyscan_callback = NULL;
static keyscan_config_t g_keyscan_config = KEYSCAN_DEFAULT_CONFIG;
#if defined(CONFIG_KEYSCAN_SUPPORT_LPM)
#define KEYSCAN_BUS_CONFIG_REG_NUM   3
#define KEYSCAN_ROW_ENABLE           0
#define KEYSCAN_COL_ENABLE           1
#define KEYSCAN_INT_ENABLE           2
static uint32_t g_keyscan_suspend_regs[KEYSCAN_BUS_CONFIG_REG_NUM] = { 0 };
#endif

#pragma weak hal_keyscan_init = hal_keyscan_v150_init
void hal_keyscan_v150_init(keyscan_pulse_time_t time, keyscan_mode_t mode, keyscan_int_t event_type,
                           hal_keyscan_callback_t callback)
{
    g_hal_keyscan_callback = callback;
    g_keyscan_config.pulse_time = time;
    g_keyscan_config.scan_mode = mode;
    g_keyscan_config.event_type = event_type;
    hal_keyscan_v150_regs_init();
    hal_keyscan_soft_rst();
    hal_keyscan_regs_set_clk_keep(0);
    hal_keyscan_pin_enable((CONFIG_KEYSCAN_ENABLE_ROW - 1), (CONFIG_KEYSCAN_ENABLE_COL - 1));
    keyscan_porting_pin_set((CONFIG_KEYSCAN_ENABLE_ROW - 1), (CONFIG_KEYSCAN_ENABLE_COL - 1));
    hal_keyscan_config_pulse_time(g_keyscan_config.pulse_time);
    hal_keyscan_config_mode(g_keyscan_config.scan_mode);
    hal_keyscan_config_direction(g_keyscan_config.direction);
    hal_keyscan_config_wait_time(g_keyscan_config.wait_time);
    hal_keyscan_config_idle_time(g_keyscan_config.idle_time);
    hal_keyscan_config_defence_num(g_keyscan_config.defence_time);
    hal_keyscan_config_ghost_check(g_keyscan_config.ghost_check);
    hal_keyscan_config_io_de(g_keyscan_config.io_de);
    hal_keyscan_config_key_select_num(CONFIG_KEYSCAN_REPORT_MAX_NUMS);
#ifdef CONFIG_KEYSCAN_ENABLE_REP_ALL_KEY
    hal_keyscan_control_key_all_report();
#endif
    keyscan_port_register_irq(KEY_SCAN_IRQN);
#if defined (CONFIG_KEYSCAN_SUPPORT_SLEEP)
    hal_keyscan_enable_int(bit(g_keyscan_config.event_type) + bit(KEYSCAN_INT_PRESS_AON));
#else
    hal_keyscan_enable_int(bit(g_keyscan_config.event_type));
#endif
}

#pragma weak hal_keyscan_deinit = hal_keyscan_v150_deinit
void hal_keyscan_v150_deinit(void)
{
    hal_keyscan_v150_disable();
    keyscan_port_unregister_irq(KEY_SCAN_IRQN);
}

#pragma weak hal_keyscan_enable = hal_keyscan_v150_enable
errcode_t hal_keyscan_v150_enable(void)
{
    hal_keyscan_regs_set_enable(1);
    hal_keyscan_regs_set_clk_ena(1);
    hal_keyscan_regs_set_start();
    for (uint32_t i = 0; i < KEYSCAN_INT_MAX_NUM; i++) {
        hal_keyscan_clr_int(bit(i));
    }
    return ERRCODE_KEYSCAN_POWER_ON;
}

#pragma weak hal_keyscan_disable = hal_keyscan_v150_disable
errcode_t hal_keyscan_v150_disable(void)
{
    hal_keyscan_regs_set_enable(0);
    hal_keyscan_regs_set_clk_ena(0);
    return ERRCODE_SUCC;
}

#if defined (CONFIG_KEYSCAN_SUPPORT_LPM)
#pragma weak hal_keyscan_suspend = hal_keyscan_v150_suspend
errcode_t hal_keyscan_v150_suspend(uintptr_t param)
{
    unused(param);
    osal_irq_disable(KEY_SCAN_IRQN);
    keyscan_porting_suspend_pin_set();
    g_keyscan_suspend_regs[KEYSCAN_ROW_ENABLE] = hal_keyscan_get_row_pin_enable();
    g_keyscan_suspend_regs[KEYSCAN_COL_ENABLE] = hal_keyscan_get_clo_pin_enable();
    g_keyscan_suspend_regs[KEYSCAN_INT_ENABLE] = hal_keyscan_get_enabled_int_val();
    return ERRCODE_SUCC;
}

#pragma weak hal_keyscan_resume = hal_keyscan_v150_resume
errcode_t hal_keyscan_v150_resume(uintptr_t param)
{
    unused(param);
    hal_keyscan_v150_regs_init();
    hal_keyscan_soft_rst();
    hal_keyscan_regs_set_clk_keep(0);
    hal_keyscan_pin_enable((uint8_t)g_keyscan_suspend_regs[KEYSCAN_ROW_ENABLE],
                           (uint8_t)g_keyscan_suspend_regs[KEYSCAN_COL_ENABLE]);
    hal_keyscan_config_pulse_time(g_keyscan_config.pulse_time);
    hal_keyscan_config_mode(g_keyscan_config.scan_mode);
    keyscan_porting_pin_resume();
    hal_keyscan_config_direction(g_keyscan_config.direction);
    hal_keyscan_config_wait_time(g_keyscan_config.wait_time);
    hal_keyscan_config_idle_time(g_keyscan_config.idle_time);
    hal_keyscan_config_defence_num(g_keyscan_config.defence_time);
    hal_keyscan_config_ghost_check(g_keyscan_config.ghost_check);
    hal_keyscan_config_io_de(g_keyscan_config.io_de);
    hal_keyscan_config_key_select_num(CONFIG_KEYSCAN_REPORT_MAX_NUMS);
#ifdef CONFIG_KEYSCAN_ENABLE_REP_ALL_KEY
    hal_keyscan_control_key_all_report();
#endif
    hal_keyscan_enable_int(g_keyscan_suspend_regs[KEYSCAN_INT_ENABLE]);
    for (uint32_t i = 0; i < KEYSCAN_INT_MAX_NUM; i++) {
        hal_keyscan_clr_int(bit(i));
    }
    hal_keyscan_regs_set_enable(1);
    hal_keyscan_regs_set_clk_ena(1);
    hal_keyscan_regs_set_start();
    osal_irq_enable(KEY_SCAN_IRQN);
    return ERRCODE_SUCC;
}
#endif

void hal_keyscan_v150_irq(void)
{
    if (!g_hal_keyscan_callback) {
        return;
    }
    for (uint32_t event_type = 0; event_type < KEYSCAN_INT_MAX_NUM; event_type++) {
        if (hal_keyscan_int_enable_check(event_type) == 1 && hal_keyscan_int_check(event_type) == 1) {
            hal_keyscan_clr_int(bit(event_type));
            g_hal_keyscan_callback(hal_keyscan_get_key_value());
        }
    }
}