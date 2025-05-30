/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: ulp_gpio file \n
 *
 * History: \n
 * 2023-10-20, Create file. \n
 */

#include "arch_port.h"
#include "chip_io.h"
#include "soc_osal.h"
#include "chip_core_irq.h"
#include "platform_core.h"
#include "debug_print.h"
#include "pinctrl_porting.h"
#include "ulp_gpio.h"

#define ULP_GPIO_BASE_ADDR              0x57030000
#define ULP_GPIO_SW_OUT_REG             (ULP_GPIO_BASE_ADDR + 0x00)
// 0：output, 1：input（默认值）
#define ULP_GPIO_SW_OEN_REG             (ULP_GPIO_BASE_ADDR + 0x04)
#define ULP_GPIO_SW_CTL_REG             (ULP_GPIO_BASE_ADDR + 0x08)
#define ULP_GPIO_INT_EN_REG             (ULP_GPIO_BASE_ADDR + 0x0C)
#define ULP_GPIO_INT_MASK_REG           (ULP_GPIO_BASE_ADDR + 0x10)
// 0：电平中断（默认值）, 1：边沿中断
#define ULP_GPIO_INT_TYPE_REG           (ULP_GPIO_BASE_ADDR + 0x14)
// 0：低电平或者下降沿（默认值）, 1：高电平或者上升沿
#define ULP_GPIO_INT_POLARITY_REG       (ULP_GPIO_BASE_ADDR + 0x18)
// 0：不使能双沿中断, 1：使能双沿中断
#define ULP_GPIO_INT_DEDGE_REG          (ULP_GPIO_BASE_ADDR + 0x1C)
// 0：不使能去毛刺（默认值）, 1：使能去毛刺
#define ULP_GPIO_INT_DEBOUNCE_REG       (ULP_GPIO_BASE_ADDR + 0x20)
#define ULP_GPIO_INT_RAM_REG            (ULP_GPIO_BASE_ADDR + 0x24)
// 中断状态寄存器(屏蔽后)
#define ULP_GPIO_INTR_REG               (ULP_GPIO_BASE_ADDR + 0x28)
// 写1清中断
#define ULP_GPIO_INT_EOI_REG            (ULP_GPIO_BASE_ADDR + 0x2C)
#define ULP_GPIO_DATA_SET_REG           (ULP_GPIO_BASE_ADDR + 0x30)
#define ULP_GPIO_DATA_CLR_REG           (ULP_GPIO_BASE_ADDR + 0x34)

#define ULP_AON_CTL_ULP_GPIO_WK_SRC_SEL0_REG    (ULP_AON_CTL_RB_ADDR + 0x6D0)
#define ULP_AON_CTL_PAD_OUT_OEH_REG             (ULP_AON_CTL_RB_ADDR + 0x820)
#define ULP_AON_CTL_PAD_OUT_OEL_REG             (ULP_AON_CTL_RB_ADDR + 0x824)
#define ULP_AON_CTL_PAD_OUT_L_REG               (ULP_AON_CTL_RB_ADDR + 0x828)
#define ULP_AON_CTL_PAD_OUT_H_REG               (ULP_AON_CTL_RB_ADDR + 0x82C)
#define ULP_AON_CTL_PAD_REG_LEN                 16
// 根据WK_SRC_SEL寄存器内的选择选取WK0或WK1
#define ULP_AON_CTL_WK0_MASK_L_REG              (ULP_AON_CTL_RB_ADDR + 0x830)
#define ULP_AON_CTL_WK0_MASK_H_REG              (ULP_AON_CTL_RB_ADDR + 0x834)
#define ULP_AON_CTL_WK1_MASK_L_REG              (ULP_AON_CTL_RB_ADDR + 0x838)
#define ULP_AON_CTL_WK1_MASK_H_REG              (ULP_AON_CTL_RB_ADDR + 0x83C)
#define ULP_AON_CTL_MASK_BITS_FULL              0xFFFF
#define ULP_AON_CTL_MASK_BITS_LEN               0x10

#define ULP_AON_CTL_ULP_GPIO_CLK_CFG_REG        (ULP_AON_CTL_RB_ADDR + 0x24)
#define PM_PCLK_INTR_SEL_BIT                    1
#define PM_PCLK_INTR_EN_BIT                     0

#define AON_GPIO_OUTPUT_LEVEL_SET_REG           (GPIO0_BASE_ADDR + 0x0)
#define AON_GPIO_OUTPUT_EN_SET_REG              (GPIO0_BASE_ADDR + 0x4)
#define AON_GPIO_OUTPUT_LEVEL_MASK_BITS_FULL    0xFFFFFFFF

#define ULP_GPIO_MAX_NUM                8
#define ULP_GPIO_MAX_WK_MUX             38

static bool g_ulp_gpio_inited = false;

static ulp_gpio_irq_cb_t g_ulp_gpio_irq_cb[ULP_GPIO_MAX_NUM] = { NULL };

static uint32_t g_aon_gpio_out_en_val = 0;
static uint32_t g_aon_gpio_out_level_val = 0;
#if defined(CONFIG_DISABLE_GPIO_INTERRUPT_DURING_PAD_SWITCHING)
static uint32_t g_ulp_gpio_enable_mask = 0;
#endif
static int ulp_gpio_irq_handler(int irq_num, const void *tmp)
{
    // 未添加锁中断，防止调用者注册耗时太久的处理函数，导致BT丢包
    unused(tmp);
    uint8_t int_sts = readb(ULP_GPIO_INTR_REG);
#if defined(PM_SLEEP_DEBUG_ENABLE) && (PM_SLEEP_DEBUG_ENABLE == YES)
    PRINT("ulp_gpio_int_sts: 0x%x\n", int_sts);
#endif
    for (uint8_t i = 0; i < ULP_GPIO_MAX_NUM; i++) {
        if ((int_sts & BIT(i)) != 0) {
            if (g_ulp_gpio_irq_cb[i] != NULL) {
                g_ulp_gpio_irq_cb[i](i);
            }
            reg8_setbit(ULP_GPIO_INT_EOI_REG, i);
        }
    }
    // 清Pending
    osal_irq_clear((uint32_t)irq_num);
    return 0;
}

void ulp_gpio_register_interrupt(void)
{
    osal_irq_clear((uint32_t)ULP_GPIO_IRQN);
    osal_irq_request(ULP_GPIO_IRQN, (osal_irq_handler)ulp_gpio_irq_handler, NULL, NULL, NULL);
    osal_irq_set_priority(ULP_GPIO_IRQN, irq_prio(ULP_GPIO_IRQN));
    osal_irq_enable(ULP_GPIO_IRQN);
}

void ulp_gpio_init(void)
{
    if (unlikely(g_ulp_gpio_inited)) {
        return;
    }
    reg16_setbit(ULP_AON_CTL_ULP_GPIO_CLK_CFG_REG, PM_PCLK_INTR_EN_BIT);
    writeb(ULP_GPIO_INT_MASK_REG, 0xFF);
    writeb(ULP_GPIO_INT_EN_REG, 0x0);
    writeb(ULP_GPIO_INT_EOI_REG, 0xFF);
    writeb(ULP_GPIO_SW_OUT_REG, 0x0);
    writeb(ULP_GPIO_SW_OEN_REG, 0xFF);
    ulp_gpio_register_interrupt();
    g_ulp_gpio_inited = true;
}

void ulp_gpio_deinit(void)
{
    if (unlikely(!g_ulp_gpio_inited)) {
        return;
    }

    writeb(ULP_GPIO_INT_MASK_REG, 0xFF);
    writeb(ULP_GPIO_INT_EN_REG, 0x0);
    writeb(ULP_GPIO_INT_EOI_REG, 0xFF);
    writeb(ULP_GPIO_SW_OUT_REG, 0x0);
    writeb(ULP_GPIO_SW_OEN_REG, 0xFF);

    for (int i = 0; i < ULP_GPIO_MAX_NUM; i++) {
        g_ulp_gpio_irq_cb[i] = NULL;
    }
    osal_irq_disable(ULP_GPIO_IRQN);
    osal_irq_free(ULP_GPIO_IRQN, NULL);
    osal_irq_clear((uint32_t)ULP_GPIO_IRQN);

    g_ulp_gpio_inited = false;
    reg16_clrbit(ULP_AON_CTL_ULP_GPIO_CLK_CFG_REG, PM_PCLK_INTR_EN_BIT);
}

void ulp_gpio_set_dir(uint8_t ulp_gpio, gpio_direction_t dir)
{
    if (unlikely(!g_ulp_gpio_inited)) {
        return;
    }
    if (dir == GPIO_DIRECTION_OUTPUT) {
        reg8_clrbit(ULP_GPIO_SW_OEN_REG, ulp_gpio);
    } else {
        reg8_setbit(ULP_GPIO_SW_OEN_REG, ulp_gpio);
    }
}

void ulp_gpio_set_val(uint8_t ulp_gpio, gpio_level_t level)
{
    if (unlikely(!g_ulp_gpio_inited)) {
        return;
    }
    if (level == GPIO_LEVEL_HIGH) {
        reg8_setbit(ULP_GPIO_SW_OUT_REG, ulp_gpio);
    } else {
        reg8_clrbit(ULP_GPIO_SW_OUT_REG, ulp_gpio);
    }
}

void ulp_gpio_enable_interrupt(uint8_t ulp_gpio)
{
    if (unlikely(!g_ulp_gpio_inited)) {
        return;
    }
    reg8_setbit(ULP_GPIO_INT_EN_REG, ulp_gpio);
}

void ulp_gpio_disable_interrupt(uint8_t ulp_gpio)
{
    if (unlikely(!g_ulp_gpio_inited)) {
        return;
    }
    reg8_setbit(ULP_GPIO_INT_EN_REG, ulp_gpio);
}

void ulp_gpio_clear_interrupt(uint8_t ulp_gpio)
{
    if (unlikely(!g_ulp_gpio_inited)) {
        return;
    }
    reg8_setbit(ULP_GPIO_INT_EOI_REG, ulp_gpio);
}

void ulp_gpio_register_irq_func(uint8_t ulp_gpio, ulp_gpio_interrupt_t trigger, ulp_gpio_irq_cb_t cb)
{
    if (unlikely(!g_ulp_gpio_inited)) {
        return;
    }
    uint32_t irq_sts = osal_irq_lock();
    // 中断屏蔽
    reg8_setbit(ULP_GPIO_INT_MASK_REG, ulp_gpio);
    // 清中断
    reg8_setbit(ULP_GPIO_INT_EOI_REG, ulp_gpio);
    // 使能去毛刺
    reg8_setbit(ULP_GPIO_INT_DEBOUNCE_REG, ulp_gpio);
    switch (trigger) {
        case ULP_GPIO_INTERRUPT_LOW:
            reg8_clrbit(ULP_GPIO_INT_TYPE_REG, ulp_gpio);
            reg8_clrbit(ULP_GPIO_INT_POLARITY_REG, ulp_gpio);
            break;

        case ULP_GPIO_INTERRUPT_HIGH:
            reg8_clrbit(ULP_GPIO_INT_TYPE_REG, ulp_gpio);
            reg8_setbit(ULP_GPIO_INT_POLARITY_REG, ulp_gpio);
            break;

        case ULP_GPIO_INTERRUPT_FALLING_EDGE:
            reg8_clrbit(ULP_GPIO_INT_DEDGE_REG, ulp_gpio);
            reg8_setbit(ULP_GPIO_INT_TYPE_REG, ulp_gpio);
            reg8_clrbit(ULP_GPIO_INT_POLARITY_REG, ulp_gpio);
            break;

        case ULP_GPIO_INTERRUPT_RISING_EDGE:
            reg8_clrbit(ULP_GPIO_INT_DEDGE_REG, ulp_gpio);
            reg8_setbit(ULP_GPIO_INT_TYPE_REG, ulp_gpio);
            reg8_setbit(ULP_GPIO_INT_POLARITY_REG, ulp_gpio);
            break;

        case ULP_GPIO_INTERRUPT_DEDGE:
            reg8_setbit(ULP_GPIO_INT_DEDGE_REG, ulp_gpio);
            reg8_setbit(ULP_GPIO_INT_TYPE_REG, ulp_gpio);
            break;

        default:
            osal_irq_restore(irq_sts);
            return;
    }
    g_ulp_gpio_irq_cb[ulp_gpio] = cb;

    // 使能中断
#if defined(CONFIG_DISABLE_GPIO_INTERRUPT_DURING_PAD_SWITCHING)
    g_ulp_gpio_enable_mask += (1 << ulp_gpio);
#endif
    reg8_setbit(ULP_GPIO_INT_EN_REG, ulp_gpio);
    // 去中断屏蔽
    reg8_clrbit(ULP_GPIO_INT_MASK_REG, ulp_gpio);
    osal_irq_restore(irq_sts);
}

void ulp_gpio_unregister_irq_func(uint8_t ulp_gpio)
{
    if (unlikely(!g_ulp_gpio_inited)) {
        return;
    }
    // 中断屏蔽
    reg8_setbit(ULP_GPIO_INT_MASK_REG, ulp_gpio);
    // 去使能中断
#if defined(CONFIG_DISABLE_GPIO_INTERRUPT_DURING_PAD_SWITCHING)
    g_ulp_gpio_enable_mask -= (1 << ulp_gpio);
#endif
    reg8_clrbit(ULP_GPIO_INT_EN_REG, ulp_gpio);
    // 清中断
    reg8_setbit(ULP_GPIO_INT_EOI_REG, ulp_gpio);
    g_ulp_gpio_irq_cb[ulp_gpio] = NULL;
}
#if defined(CONFIG_DISABLE_GPIO_INTERRUPT_DURING_PAD_SWITCHING)
void ulp_gpio_int_enable(void)
{
    writew(ULP_GPIO_INT_EN_REG, g_ulp_gpio_enable_mask);
}

void ulp_gpio_int_disable(void)
{
    writew(ULP_GPIO_INT_EN_REG, 0);
}
#endif
/**
 * 0-31：gpio独立选择
 * 32：swd_clk管脚
 * 33：swd_io管脚
 * 34：gpio或mask0
 * 35：gpio与mask0
 * 36：gpio或mask1
 * 37：gpio或mask1
 */
void ulp_gpio_wk_src_sel(uint8_t ulp_gpio, uint8_t wk_mux)
{
    if (unlikely(!g_ulp_gpio_inited)) {
        return;
    }
    uint32_t addr = (ULP_AON_CTL_ULP_GPIO_WK_SRC_SEL0_REG + (ulp_gpio / 0x2) * 0x4);    // 计算地址
    if ((ulp_gpio % 0x2) == 0) {
        reg16_setbits(addr, 0x0, 0x6, wk_mux);
    } else {
        reg16_setbits(addr, 0x8, 0x6, wk_mux);
    }
}

void ulp_gpio_mask_pin_set(ulp_gpio_wk_t wk_sel, uint32_t pin_map)
{
    switch (wk_sel) {
        case ULP_GPIO_WK0_MASK:
            writew(ULP_AON_CTL_WK0_MASK_H_REG, pin_map >> ULP_AON_CTL_MASK_BITS_LEN);
            writew(ULP_AON_CTL_WK0_MASK_L_REG, pin_map & ULP_AON_CTL_MASK_BITS_FULL);
            break;

        case ULP_GPIO_WK1_MASK:
            writew(ULP_AON_CTL_WK1_MASK_H_REG, pin_map >> ULP_AON_CTL_MASK_BITS_LEN);
            writew(ULP_AON_CTL_WK1_MASK_L_REG, pin_map & ULP_AON_CTL_MASK_BITS_FULL);
            break;

        default:
            return;
    }
}

void pin_pad_out_config_during_sleep(pin_t pin, bool oe, bool out_high)
{
    uint32_t oe_addr, out_addr;
    uint8_t bit = pin;
    if (pin < 16) { // < 16: s_mgpio0~15
        oe_addr = ULP_AON_CTL_PAD_OUT_OEL_REG;
        out_addr = ULP_AON_CTL_PAD_OUT_L_REG;
    } else {
        oe_addr = ULP_AON_CTL_PAD_OUT_OEH_REG;
        out_addr = ULP_AON_CTL_PAD_OUT_H_REG;
        bit -= 16;  // s_mgpio16~31: bit0~15
    }
    if (!oe) {
        reg16_clrbit(oe_addr, bit);
    } else if (out_high) {
        reg16_setbit(out_addr, bit);
        reg16_setbit(oe_addr, bit);
    } else {
        reg16_clrbit(out_addr, bit);
        reg16_setbit(oe_addr, bit);
    }
}

void pm_aon_gpio_suspend(void)
{
    g_aon_gpio_out_en_val = (~readl(AON_GPIO_OUTPUT_EN_SET_REG)) & pin_gpio_mode_get();
    g_aon_gpio_out_level_val = readl(AON_GPIO_OUTPUT_LEVEL_SET_REG) & pin_gpio_mode_get();
    uint16_t ulp_gpio_out_en_l = (g_aon_gpio_out_en_val & ULP_AON_CTL_MASK_BITS_FULL) |
                                  readw(ULP_AON_CTL_PAD_OUT_OEL_REG);
    uint16_t ulp_gpio_out_en_h = (g_aon_gpio_out_en_val >> ULP_AON_CTL_PAD_REG_LEN) |
                                  readw(ULP_AON_CTL_PAD_OUT_OEH_REG);
    uint16_t ulp_gpio_out_l = (g_aon_gpio_out_level_val & ULP_AON_CTL_MASK_BITS_FULL) |
                               readw(ULP_AON_CTL_PAD_OUT_L_REG);
    uint16_t ulp_gpio_out_h = (g_aon_gpio_out_level_val >> ULP_AON_CTL_PAD_REG_LEN) |
                               readw(ULP_AON_CTL_PAD_OUT_H_REG);
    writew(ULP_AON_CTL_PAD_OUT_OEL_REG, ulp_gpio_out_en_l);
    writew(ULP_AON_CTL_PAD_OUT_OEH_REG, ulp_gpio_out_en_h);
    writew(ULP_AON_CTL_PAD_OUT_L_REG, ulp_gpio_out_l);
    writew(ULP_AON_CTL_PAD_OUT_H_REG, ulp_gpio_out_h);
}

void pm_aon_gpio_resume(void)
{
    writel(AON_GPIO_OUTPUT_EN_SET_REG, (~g_aon_gpio_out_en_val));
    writel(AON_GPIO_OUTPUT_LEVEL_SET_REG, g_aon_gpio_out_level_val & AON_GPIO_OUTPUT_LEVEL_MASK_BITS_FULL);
}

void pm_clr_ulp_gpio(void)
{
    writel(ULP_AON_CTL_PAD_OUT_OEL_REG, 0);
    writew(ULP_AON_CTL_PAD_OUT_OEH_REG, 0);
    writew(ULP_AON_CTL_PAD_OUT_L_REG, 0);
    writew(ULP_AON_CTL_PAD_OUT_H_REG, 0);
}

void ulp_gpio_int_wkup_config(ulp_gpio_int_wkup_cfg_t *cfg, uint8_t array_num)
{
    if (unlikely(!g_ulp_gpio_inited)) {
        return;
    }
    for (int i = 0; i < array_num; i++) {
        if (cfg[i].ulp_gpio >= ULP_GPIO_MAX_NUM) {
            continue;
        }
        ulp_gpio_wk_src_sel(cfg[i].ulp_gpio, cfg[i].wk_mux);
        if (cfg[i].int_enable) {
            ulp_gpio_register_irq_func(cfg[i].ulp_gpio, cfg[i].trigger, cfg[i].irq_cb);
        } else {
            ulp_gpio_unregister_irq_func(cfg[i].ulp_gpio);
        }
    }
}
