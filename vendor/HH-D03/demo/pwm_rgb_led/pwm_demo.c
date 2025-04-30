/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: PWM Sample Source. \n
 *
 * History: \n
 * 2023-06-27, Create file. \n
 */
#if defined(CONFIG_PWM_SUPPORT_LPM)
#include "pm_veto.h"
#endif
#include "common_def.h"
#include "pinctrl.h"
#include "pwm.h"
#include "soc_osal.h"
#include "app_init.h"
#include "gpio.h"
#define TEST_TCXO_DELAY_1000MS 1000

#define PWM_TASK_PRIO 24
#define PWM_TASK_STACK_SIZE 0x1000
#define CONFIG_PWM1_PIN 9
#define CONFIG_PWM1_CHANNEL 1
#define CONFIG_PWM1_PIN_MODE 41
#define CONFIG_PWM1_GROUP_ID 0
#define COLOR 255
#define DELAY_MS 1000

/* 根据数据手册可以得出T为1.2US，频率为833333hz, */
void set_code0(void)
{
    pwm_config_t cfg_no_repeat = {52, 12, 0, 1, false};
    uapi_pwm_open(CONFIG_PWM1_CHANNEL, &cfg_no_repeat);
    uint8_t channel_id = CONFIG_PWM1_CHANNEL;
    uapi_pwm_set_group(CONFIG_PWM1_GROUP_ID, &channel_id, 1);
    uapi_pwm_start_group(CONFIG_PWM1_GROUP_ID);
    uapi_pwm_close(CONFIG_PWM1_GROUP_ID);
}

void set_code1(void)
{
    pwm_config_t cfg_no_repeat = {32, 32, 0, 1, false};
    uapi_pwm_open(CONFIG_PWM1_CHANNEL, &cfg_no_repeat);
    uint8_t channel_id = CONFIG_PWM1_CHANNEL;
    uapi_pwm_set_group(CONFIG_PWM1_GROUP_ID, &channel_id, 1);
    uapi_pwm_start_group(CONFIG_PWM1_GROUP_ID);
    uapi_pwm_close(CONFIG_PWM1_GROUP_ID);
}
static void pwm_color(uint8_t red, uint8_t green, uint8_t blue)
{
    int code;
    for (int bit = 0; bit < 8; bit++) { // 8代表每个颜色8bit控制
        code = (green << bit) & 0x80;
        if (code != 0) {
            set_code1();
        } else {
            set_code0();
        }
    }

    for (int bit = 0; bit < 8; bit++) { // 8代表每个颜色8bit控制
        code = (red << bit) & 0x80;
        if (code != 0) {
            set_code1();
        } else {
            set_code0();
        }
    }

    for (int bit = 0; bit < 8; bit++) { // 8代表每个颜色8bit控制
        code = (blue << bit) & 0x80;
        if (code != 0) {
            set_code1();
        } else {
            set_code0();
        }
    }
}

static void *pwm_task(void)
{
    osal_printk("PWM start. \r\n");
    uapi_pin_set_mode(CONFIG_PWM1_PIN, CONFIG_PWM1_PIN_MODE);
    uapi_pwm_deinit();
    uapi_pwm_init();
    while (1) {
        pwm_color(COLOR, 0, 0);
        osal_msleep(DELAY_MS);
        pwm_color(0, COLOR, 0);
        osal_msleep(DELAY_MS);
        pwm_color(0, 0, COLOR);
        osal_msleep(DELAY_MS);
    }
    return NULL;
}

static void pwm_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)pwm_task, 0, "PwmTask", PWM_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, PWM_TASK_PRIO);
    }
    osal_kthread_unlock();
}

/* Run the pwm_entry. */
app_run(pwm_entry);