/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides watchdog port template \n
 *
 * History: \n
 * 2022-06-06， Create file. \n
 */
#include "watchdog.h"
#include "tcxo.h"
#include "platform_core.h"
#include "chip_io.h"
#include "hal_watchdog_v151.h"
#include "soc_osal.h"
#include "watchdog_porting.h"

#define PARAM       0
#define WAIT_APPS_DUMP_DSP_DELAY_MS  15000
#define WDT_PORTING_ENABLE_DELAY     60uLL
#define DEFAULT_WDT_TIME             8
#define ULP_WDT_EN                   0x5702C930
#define ULP_WDT_EN_BIT               0
#define ULP_WDT_CTRL                 0x5702C938
#define ULP_WDT_TIME_LEN             2
#define ULP_WDT_KICK                 0x5702C934
#define ULP_WDT_KICK_VALUE           0xC5

uintptr_t g_watchdog_base_addr = (uintptr_t)CHIP_WDT_BASE_ADDRESS;
static int32_t g_watchdog_config = DEFAULT_WDT_TIME ;
static bool g_ulp_wdt_inited = false;

void watchdog_port_register_hal_funcs(void)
{
    hal_watchdog_register_funcs(hal_watchdog_v151_funcs_get());
}

void watchdog_port_unregister_hal_funcs(void)
{
    hal_watchdog_unregister_funcs();
}

void watchdog_port_register_irq(void)
{
    return;
}

void irq_wdt_handler(void)
{
    hal_watchdog_v151_irq_handler((uintptr_t)PARAM);
}

/* ------------------------------------------------------------------------------*/
/* The following is the product adaptation section. */
/* ------------------------------------------------------------------------------*/


void watchdog_func_adapt(uint32_t time)
{
    watchdog_set_config_time(time);
}

void watchdog_turnon_clk(void)
{
    reg_setbit(HAL_SOFT_RST_CTL_BASE, HAL_GLB_CTL_M_ATOP1_L_REG_OFFSET, HAL_CHIP_WDT_ATOP1_RST_BIT);
    uapi_tcxo_delay_us(WDT_PORTING_ENABLE_DELAY);
}

void watchdog_turnoff_clk(void)
{
    reg_clrbit(HAL_SOFT_RST_CTL_BASE, HAL_GLB_CTL_M_ATOP1_L_REG_OFFSET, HAL_CHIP_WDT_ATOP1_RST_BIT);
}

void watchdog_set_config_time(uint32_t time)
{
    g_watchdog_config = time;
}

uint32_t watchdog_get_config_time(void)
{
    return g_watchdog_config;
}

errcode_t ulp_wdt_init(ulp_wdt_timeout_t timeout)
{
    if (timeout > ULP_WDT_TIMEOUT_MAX) {
        return ERRCODE_FAIL;
    }

    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(g_ulp_wdt_inited == true)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }
    reg8_clrbit(ULP_WDT_EN, ULP_WDT_EN_BIT);
    reg8_setbits(ULP_WDT_CTRL, 0, ULP_WDT_TIME_LEN, (uint8_t)timeout);
    reg8_setbit(ULP_WDT_EN, ULP_WDT_EN_BIT);
    g_ulp_wdt_inited = true;
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t ulp_wdt_deinit(void)
{
    uint32_t irq_sts = osal_irq_lock();
    if (unlikely(g_ulp_wdt_inited == false)) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }
    reg8_clrbit(ULP_WDT_EN, ULP_WDT_EN_BIT);

    g_ulp_wdt_inited = false;
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t ulp_wdt_kick(void)
{
    if (unlikely(g_ulp_wdt_inited == false)) {
        return ERRCODE_FAIL;
    }

    unsigned int irq_sts = osal_irq_lock();
    writeb(ULP_WDT_KICK, ULP_WDT_KICK_VALUE);
    osal_irq_restore(irq_sts);

    return ERRCODE_SUCC;
}

errcode_t ulp_wdt_timeout_config(ulp_wdt_timeout_t timeout)
{
    if (unlikely(g_ulp_wdt_inited == false)) {
        return ERRCODE_FAIL;
    }
    reg8_setbits(ULP_WDT_CTRL, 0, ULP_WDT_TIME_LEN, (uint8_t)timeout);
    writeb(ULP_WDT_KICK, ULP_WDT_KICK_VALUE);

    return ERRCODE_SUCC;
}