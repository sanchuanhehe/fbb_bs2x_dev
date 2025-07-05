/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse Button source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */

#include "osal_debug.h"
#include "gpio.h"
#include "pinctrl.h"
#include "tcxo.h"
#include "mouse_button.h"

/**
 * @def PIN_LEFT
 * @brief 鼠标左键GPIO引脚
 */
#define PIN_LEFT CONFIG_MOUSE_PIN_LEFT

/**
 * @def PIN_RIGHT
 * @brief 鼠标右键GPIO引脚
 */
#define PIN_RIGHT CONFIG_MOUSE_PIN_RIGHT

/**
 * @def PIN_MID
 * @brief 鼠标中键GPIO引脚
 */
#define PIN_MID CONFIG_MOUSE_PIN_MID

/**
 * @def DELAY_US200
 * @brief 按键消抖延时，单位为微秒
 */
#define DELAY_US200 200

/**
 * @brief 全局鼠标按键状态指针
 */
static mouse_key_t *g_mouse_key = NULL;

/**
 * @brief 鼠标左键中断回调函数
 * @param pin 触发中断的GPIO引脚
 */
static void mouse_left_button_func(pin_t pin)
{
    osal_printk("Left button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    if (g_mouse_key != NULL) {
        g_mouse_key->b.left_key = !uapi_gpio_get_val(pin);
    }
}

/**
 * @brief 鼠标右键中断回调函数
 * @param pin 触发中断的GPIO引脚
 */
static void mouse_right_button_func(pin_t pin)
{
    osal_printk("Right button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    if (g_mouse_key != NULL) {
        g_mouse_key->b.right_key = !uapi_gpio_get_val(pin);
    }
}

/**
 * @brief 鼠标中键中断回调函数
 * @param pin 触发中断的GPIO引脚
 */
static void mouse_mid_button_func(pin_t pin)
{
    osal_printk("MID button clicked.\r\n");
    uapi_tcxo_delay_us(DELAY_US200);
    if (g_mouse_key != NULL) {
        g_mouse_key->b.mid_key = !uapi_gpio_get_val(pin);
    }
}

/**
 * @brief 鼠标按键初始化
 * @param mouse_key 鼠标按键状态结构体指针
 */
void mouse_button_init(mouse_key_t *mouse_key)
{
    g_mouse_key = mouse_key;

    // 初始化鼠标按键状态
    if (g_mouse_key != NULL) {
        g_mouse_key->b.left_key = 0;
        g_mouse_key->b.right_key = 0;
        g_mouse_key->b.mid_key = 0;
        g_mouse_key->b.reserved = 0;
    } else {
        osal_printk("Error: mouse_key pointer is NULL.\r\n");
        return;
    }

    uapi_gpio_init();

    uapi_pin_set_mode(PIN_LEFT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(PIN_RIGHT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(PIN_MID, (pin_mode_t)HAL_PIO_FUNC_GPIO);

    uapi_pin_set_pull(PIN_LEFT, PIN_PULL_UP);
    uapi_pin_set_pull(PIN_RIGHT, PIN_PULL_UP);
    uapi_pin_set_pull(PIN_MID, PIN_PULL_UP);

    gpio_select_core(PIN_LEFT, CORES_APPS_CORE);
    gpio_select_core(PIN_RIGHT, CORES_APPS_CORE);
    gpio_select_core(PIN_MID, CORES_APPS_CORE);

    uapi_gpio_set_dir(PIN_LEFT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(PIN_RIGHT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(PIN_MID, GPIO_DIRECTION_INPUT);

    uapi_gpio_register_isr_func(PIN_LEFT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_left_button_func);
    uapi_gpio_register_isr_func(PIN_RIGHT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_right_button_func);
    uapi_gpio_register_isr_func(PIN_MID, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_mid_button_func);
}