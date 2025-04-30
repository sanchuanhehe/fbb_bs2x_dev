/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Test mouse GPIO source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-10, Create file. \n
 */
#include "gpio.h"
#include "pinctrl.h"
#include "tcxo.h"
#include "osal_debug.h"
#include "usb_button_demo.h"

#define DELAY_US200 200
#define BUTTON_GPIO 14 // 按键

static usb_hid_mouse_key_t *g_mouse_key = NULL;

static void mouse_left_button_func(pin_t pin, uintptr_t param)
{
    UNUSED(param);
    osal_printk("Left button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    if (g_mouse_key != NULL) {
        g_mouse_key->b.right_key = !uapi_gpio_get_val(pin);
    }
}

void mouse_button_init(usb_hid_mouse_key_t *mouse_key)
{
    g_mouse_key = mouse_key;

    uapi_gpio_init();
    uapi_pin_set_mode(BUTTON_GPIO, HAL_PIO_FUNC_GPIO);
    uapi_pin_set_pull(BUTTON_GPIO, (pin_pull_t)HAL_PIO_PULL_UP);
    uapi_gpio_set_dir(BUTTON_GPIO, GPIO_DIRECTION_INPUT);
    errcode_t ret = uapi_gpio_register_isr_func(BUTTON_GPIO, GPIO_INTERRUPT_DEDGE, mouse_left_button_func);
    if (ret != 0) {
        printf("ddadada\r\n");
        uapi_gpio_unregister_isr_func(BUTTON_GPIO);
    }
}