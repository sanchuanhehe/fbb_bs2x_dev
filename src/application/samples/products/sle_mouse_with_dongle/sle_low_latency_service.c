/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Sle Low Latency Mouse Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include <los_swtmr.h>
#include "soc_osal.h"
#include "osal_debug.h"
#include "securec.h"
#include "sle_low_latency.h"
#include "qdec.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "gpio.h"
#include "tcxo.h"
#include "arch_barrier.h"
#include "watchdog_porting.h"
#include "watchdog.h"
#include "gpio.h"
#include "adc.h"
#include "adc_porting.h"
#include "timer.h"
#include "chip_core_irq.h"
#include "gadget/f_hid.h"
#include "mouse_usb/usb_init_app.h"
#include "mouse_sensor/mouse_sensor.h"
#include "mouse_button/mouse_button.h"
#include "sle_ssap_server.h"
#include "sle_connection_manager.h"
#include "sle_errcode.h"
#include "sle_mouse_server/sle_mouse_server.h"
#include "errcode.h"
#include "usb_porting.h"
#include "sle_low_latency_service.h"
// #define CONFIG_MOUSE_ADC_VBAT_CH 0
#define GAFE_SAMPLE_VALUE_SIGN_BIT 17
#define VBAT_SAMPLE_INTERVAL_US 30000000
#define ADC_REFERENCE_VOLTAGE_MV 1500
#define ADC_REF_VOL_DIFFERENCE_MULT 2
#define ADC_TICK2VOL_REF_VOLTAGE_MV (ADC_REFERENCE_VOLTAGE_MV * ADC_REF_VOL_DIFFERENCE_MULT)
#define SPI_RECV_DATA_LEN 1
#define SPI_SEND_DATA_LEN 2
#define MOUSE_TO_BT_DATA_LEN 5
#define DELAY_MS3 3
#define MOUSE_DELAY_J_NUM 40
#define MOUSE_DELAY_1US 1
#define MOUSE_DELAY_2US 2
#define MOUSE_DELAY_6US 6
#define TURNOVER_SIGN (-1)
#define DELAY_US200 200
#define SEND_MOUSE_MSG_TEST 100
#define TEST_USB_TIMER_MS 2
#define WHEEL_DATA 4
#define USB_MOUSE_REPORTER_LEN 5
#define MS_PER_S 1000
#define USB_HID_MOUSE_SIM_SEND_DELAY_MS (500UL)
#define USB_HID_MOUSE_SIM_SEND_DELAY_US (500UL)
#define USB_HID_MOUSE_INIT_DELAY_MS (500UL)
#define BLE_HID_MOUSE_HIGH_XY_BIT 12
#define USB_MOUSE_DRAW_QUADRATE_ANGLE 4
#define USB_MOUSE_DRAW_QUADRATE_TIMES 100
#define MOUSE_INPUT_MOVE_LEFT 200
#define MOUSE_INPUT_MOVE_RIGHT 100
#define MOUSE_INPUT_MOVE_UP 200
#define MOUSE_INPUT_MOVE_DOWN 100
#define MOUSE_INPUT_ROLL_FORWARD 200
#define MOUSE_INPUT_ROLL_BACK 100
#define MOUSE_MOVE_STEP 100
#define MOUSE_ROLL_STEP 100
#define MOUSE_INPUT_NUM 6
#define MOUSE_SIM_TIME 1
#define MOUSE_INPUT_KEY 0
#define MOUSE_INPUT_X 1
#define MOUSE_INPUT_Y 2
#define MOUSE_INPUT_WHEEL 3
#define SIMULATE_TIMES (380)
#define DELAY_MS (1000)
#define DELAY_INPUT (20)
#define USB_MOUSE_POLLING_RATE_MAX_BIT 4
#define USB_MOUSE_POLLING_RATE_MAX_VALUE 8
#define MOUSE_KEY_UP_DELAY 10
#define MOUSE_KEY_CLICK_DELAY 200
#define DATA_BIT2 2
#define DATA_BIT3 3
#define DATA_BIT4 4
#define DATA_BIT8 8
#define MOUSE_KIND 0x2
#define SLE_MOUSE_TASK_DELAY_300_MS 300
#define SLE_MOUSE_TASK_DELAY_1700_MS 1700
#ifdef CONFIG_SAMPLE_SLE_DONGLE_1K
#define REPORT_TIME 7
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_2K)
#define REPORT_TIME 3
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_4K)
#define REPORT_TIME 1
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_8K)
#define REPORT_TIME 0
#else
#define REPORT_TIME 7
#endif

#define DEBUG
uint8_t g_report_time = REPORT_TIME;

typedef struct usb_hid_mouse_report {
    uint8_t kind;
    mouse_key_t key;
    int8_t x;
    int8_t y;
    int8_t wheel;
} usb_hid_mouse_report_t;

ssap_mouse_key_t g_mouse_notify_data = {0};
#pragma pack(1)
typedef struct {
    int32_t button_mask : 8;
    int32_t x : 12; /* mouse x */
    int32_t y : 12; /* mouse y */
    int8_t wheel;
} low_latency_mouse_t;
#pragma pack()

static mouse_sensor_oprator_t g_usb_hid_hs_mouse_operator = {0};
static usb_hid_mouse_report_t g_send_mouse_msg = {0};
static qdec_config_t g_usb_qdec_config = QDEC_DEFAULT_CONFIG;
static int g_usb_mouse_hid_index;
static uint8_t g_dongle_data_buffer[32] = {0};  // 存储接收到的低延迟数据
static bool g_dongle_data_ready = false;

/**
 * @brief 发送鼠标消息到SSAP
 * @return true 发送成功
 * @return false 发送失败
 */
static bool sle_send_msg(void)
{
    uint8_t conn_state = SLE_ACB_STATE_NONE;
    uint32_t pair_status = ERRCODE_SLE_FAIL;
    bool ssap_able = false;
    get_g_sle_mouse_server_conn_state(&conn_state);
    get_g_sle_mouse_pair_state(&pair_status);
    if (conn_state != SLE_ACB_STATE_CONNECTED || pair_status != ERRCODE_SLE_SUCCESS) {
        return false;
    }
    get_g_read_ssap_support(&ssap_able);
    if (ssap_able == true) {
        sle_hid_mouse_server_send_input_report(&g_mouse_notify_data);
        osal_msleep(SLE_MOUSE_TASK_DELAY_20_MS);
    }
    return true;
}

/**
 * @brief 鼠标左键中断回调函数
 * @param[in] pin 左键GPIO引脚
 */
static void mouse_left_button_func(pin_t pin)
{
#ifdef DEBUG
    osal_printk("mouse_left_button_func: pin %d, val %d\n", pin, uapi_gpio_get_val(pin));
#endif
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.left_key = !uapi_gpio_get_val(pin);
    g_mouse_notify_data.button_mask = g_send_mouse_msg.key.d8;
    
#ifdef DEBUG
    osal_printk("Left button: key_mask=0x%02x, pressed=%d\n", g_mouse_notify_data.button_mask, g_send_mouse_msg.key.b.left_key);
#endif
    
    // 注意：不再直接调用sle_send_msg()，而是让低延迟系统通过sle_mouse_key_set回调来获取数据
    // 数据会被低延迟系统自动获取并通过低延迟通道发送
    
    uapi_gpio_clear_interrupt(pin);
}

/**
 * @brief 鼠标右键中断回调函数
 * @param[in] pin 右键GPIO引脚
 */
static void mouse_right_button_func(pin_t pin)
{
#ifdef DEBUG
    osal_printk("mouse_right_button_func: pin %d, val %d\n", pin, uapi_gpio_get_val(pin));
#endif
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.right_key = !uapi_gpio_get_val(pin);
    g_mouse_notify_data.button_mask = g_send_mouse_msg.key.d8;
    
#ifdef DEBUG
    osal_printk("Right button: key_mask=0x%02x, pressed=%d\n", g_mouse_notify_data.button_mask, g_send_mouse_msg.key.b.right_key);
#endif
    
    // 注意：不再直接调用sle_send_msg()，而是让低延迟系统通过sle_mouse_key_set回调来获取数据
    // 数据会被低延迟系统自动获取并通过低延迟通道发送
    
    uapi_gpio_clear_interrupt(pin);
}

/**
 * @brief 鼠标中键中断回调函数
 * @param[in] pin 中键GPIO引脚
 */
static void mouse_mid_button_func(pin_t pin)
{
#ifdef DEBUG
    osal_printk("mouse_mid_button_func: pin %d, val %d\n", pin, uapi_gpio_get_val(pin));
#endif
    uapi_tcxo_delay_us(DELAY_US200);
    g_send_mouse_msg.key.b.mid_key = !uapi_gpio_get_val(pin);
    g_mouse_notify_data.button_mask = g_send_mouse_msg.key.d8;
    
#ifdef DEBUG
    osal_printk("Mid button: key_mask=0x%02x, pressed=%d\n", g_mouse_notify_data.button_mask, g_send_mouse_msg.key.b.mid_key);
#endif
    
    // 注意：不再直接调用sle_send_msg()，而是让低延迟系统通过sle_mouse_key_set回调来获取数据
    // 数据会被低延迟系统自动获取并通过低延迟通道发送
    
    uapi_gpio_clear_interrupt(pin);
}

/**
 * @brief QDEC编码器回调
 * @param[in] argc 编码器变化量
 * @param[in] argv 未使用
 * @return 总是返回0
 */
static int qdec_report_callback(int argc, char *argv[])
{
    UNUSED(argv);
    g_send_mouse_msg.wheel = -argc;
    g_mouse_notify_data.wheel += g_send_mouse_msg.wheel;
    if (sle_send_msg()) {
        g_mouse_notify_data.wheel = 0;
    }
    osal_irq_clear(QDEC_IRQN);
    return 0;
}

/**
 * @brief 鼠标IO初始化，包括按键和QDEC相关引脚
 */
static void mouse_io_init(void)
{
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_LEFT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_RIGHT, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_mode(CONFIG_MOUSE_PIN_MID, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_LEFT, PIN_PULL_UP);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_RIGHT, PIN_PULL_UP);
    uapi_pin_set_pull(CONFIG_MOUSE_PIN_MID, PIN_PULL_UP);

    gpio_select_core(CONFIG_MOUSE_PIN_LEFT, CORES_APPS_CORE);
    gpio_select_core(CONFIG_MOUSE_PIN_RIGHT, CORES_APPS_CORE);
    gpio_select_core(CONFIG_MOUSE_PIN_MID, CORES_APPS_CORE);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_LEFT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_RIGHT, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_MID, GPIO_DIRECTION_INPUT);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_LEFT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_left_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_RIGHT, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_right_button_func);
    uapi_gpio_register_isr_func(CONFIG_MOUSE_PIN_MID, GPIO_INTERRUPT_DEDGE, (gpio_callback_t)mouse_mid_button_func);

    uapi_pin_set_mode(CONFIG_MOUSE_PIN_QDEC_COMMON, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    gpio_select_core(CONFIG_MOUSE_PIN_QDEC_COMMON, CORES_APPS_CORE);
    uapi_gpio_set_dir(CONFIG_MOUSE_PIN_QDEC_COMMON, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_MOUSE_PIN_QDEC_COMMON, 0);
}

/**
 * @brief 电池电压采样定时器回调
 * @param[in] data 定时器句柄
 */
static void vbat_sample_cb(uintptr_t data)
{
    int adc_value = 0;
    adc_value = uapi_adc_auto_sample(CONFIG_MOUSE_ADC_VBAT_CH);
    osal_printk("VBAT: %dmv\n", (adc_value * ADC_TICK2VOL_REF_VOLTAGE_MV) >> GAFE_SAMPLE_VALUE_SIGN_BIT);
    osal_irq_clear(TIMER_0_IRQN);
    uapi_timer_start((timer_handle_t)data, VBAT_SAMPLE_INTERVAL_US, vbat_sample_cb, data);
}

/**
 * @brief 初始化电池电压ADC采样
 */
void vbat_adc_init(void)
{
    uapi_pin_set_mode(CONFIG_MOUSE_ADC_VBAT_CH, PIN_MODE_0);
    uapi_gpio_set_dir(CONFIG_MOUSE_ADC_VBAT_CH, GPIO_DIRECTION_INPUT);
    uapi_pin_set_pull(CONFIG_MOUSE_ADC_VBAT_CH, PIN_PULL_NONE);
#if defined(CONFIG_PINCTRL_SUPPORT_IE)
    uapi_pin_set_ie(CONFIG_MOUSE_ADC_VBAT_CH, PIN_IE_1);
#endif
    uapi_adc_init(ADC_CLOCK_NONE);
    uapi_adc_power_en(AFE_GADC_MODE, true);
    uapi_adc_open_channel(CONFIG_MOUSE_ADC_VBAT_CH);
    adc_calibration(AFE_GADC_MODE, true, true, true);
    static timer_handle_t timer = 0;
    uapi_timer_create(DEFAULT_TIMER, &timer);
    uapi_timer_start(timer, VBAT_SAMPLE_INTERVAL_US, vbat_sample_cb, (uintptr_t)timer);
}

/**
 * @brief 鼠标初始化，包括IO、ADC、QDEC、传感器等
 * @param[in] sensor_id 传感器ID
 * @return mouse_freq_t 鼠标工作频率
 */
mouse_freq_t mouse_init(uint32_t sensor_id)
{
    mouse_io_init();
    vbat_adc_init();
    uapi_qdec_init(&g_usb_qdec_config);
    qdec_port_pinmux_init(CONFIG_MOUSE_PIN_QDEC_A, CONFIG_MOUSE_PIN_QDEC_B);
    uapi_qdec_register_callback(qdec_report_callback);
    uapi_qdec_enable();
    osal_printk("sensor:%d\r\n", sensor_id);
    g_usb_hid_hs_mouse_operator = get_mouse_sensor_operator(sensor_id);
    mouse_freq_t freq = g_usb_hid_hs_mouse_operator.init();
    osal_printk("g_usb_hid_hs_mouse_operator set frequency :%d\r\n", freq);
    osal_printk("mouse init done\r\n");
    return freq;
}

/**
 * @brief 获取鼠标按键信息
 * @param[out] button_mask 按键掩码
 * @param[out] x X轴坐标
 * @param[out] y Y轴坐标
 * @param[out] wheel 滚轮值
 * @return errcode_t 操作结果
 */
static errcode_t sle_mouse_key_set(int8_t *button_mask, int16_t *x, int16_t *y, int8_t *wheel)
{
    g_usb_hid_hs_mouse_operator.get_xy(x, y);
    *button_mask = g_send_mouse_msg.key.d8;
    *wheel = g_send_mouse_msg.wheel;
    g_send_mouse_msg.wheel = 0;
    
#ifdef DEBUG
    // 打印每次调用情况（用于调试低延迟回调是否被正确调用）
    static uint32_t call_count = 0;
    call_count++;
    
    // 只在有移动或按键时打印详细信息
    if (*x != 0 || *y != 0 || *button_mask != 0 || *wheel != 0) {
        osal_printk("sle_mouse_key_set[%d]: button=0x%x, x=%d, y=%d, wheel=%d\n", 
                   call_count, *button_mask, *x, *y, *wheel);
    } else if (call_count % 1000 == 0) {
        // 每1000次调用打印一次，确认回调在正常工作
        osal_printk("sle_mouse_key_set[%d]: callback working (no data)\n", call_count);
    }
#endif
    
    // 返回 ERRCODE_SUCC 表示有数据需要发送
    // 如果没有数据变化，可以考虑返回 SLE_LOW_LATENCY_VALUE_GET_FAIL 来避免发送
    if (*x != 0 || *y != 0 || *button_mask != 0 || *wheel != 0) {
        return ERRCODE_SUCC;  // 有数据，发送
    } else {
        // 对于鼠标应用，即使没有变化也应该定期发送状态
        return ERRCODE_SUCC;  // 继续发送，保持连接活跃
    }
}

/**
 * @brief 获取鼠标按键并发送输入报告
 */
void sle_mouse_get_key(void)
{
    int8_t button_mask = 0;
    int16_t x = 0;
    int16_t y = 0;
    int8_t wheel = 0;
    sle_mouse_key_set(&button_mask, &x, &y, &wheel);
    g_mouse_notify_data.button_mask = button_mask;
    g_mouse_notify_data.x = x;
    g_mouse_notify_data.y = y;
    g_mouse_notify_data.wheel = wheel;
    sle_hid_mouse_server_send_input_report(&g_mouse_notify_data);
}

/**
 * @brief 低延迟数据回调函数（Dongle端接收数据）
 * @param[in] data 接收到的数据
 * @param[in] len 数据长度
 */
static void dongle_low_latency_report_callback(uint8_t *data, uint8_t len)
{
    osal_printk("dongle_low_latency_report_callback: received %d bytes\n", len);
    
    if (data == NULL || len == 0 || len > sizeof(g_dongle_data_buffer)) {
        osal_printk("dongle_low_latency_report_callback: invalid data, len=%d\n", len);
        return;
    }
    
    // 将接收到的数据缓存起来
    if (memcpy_s(g_dongle_data_buffer, sizeof(g_dongle_data_buffer), data, len) != EOK) {
        osal_printk("dongle_low_latency_report_callback: memcpy failed\n");
        return;
    }
    
    g_dongle_data_ready = true;
    
#ifdef DEBUG
    // 打印接收到的原始数据
    osal_printk("dongle_low_latency_report_callback: raw data = ");
    for (uint8_t i = 0; i < len; i++) {
        osal_printk("%02x ", data[i]);
    }
    osal_printk("\n");
#endif
}

/**
 * @brief 设置EM数据回调（用于低延迟模式控制）
 * @param[in] co_handle 连接句柄
 * @param[in] status 状态
 */
static void sle_mouse_set_em_data_cbk(uint16_t co_handle, uint8_t status)
{
    unused(co_handle);
    unused(status);
    osal_printk("sle_mouse_set_em_data_cbk: co_handle=%d, status=%d\n", co_handle, status);
}

/**
 * @brief Sle低延迟鼠标应用初始化，注册回调
 */
void sle_low_latency_mouse_app_init(void)
{
    osal_printk("sle_low_latency_mouse_app_init: starting mouse callback registration\n");
    
    // 注册mouse专用的低延迟回调
    sle_low_latency_mouse_callbacks_t mouse_cbk;
    mouse_cbk.set_value_cb = sle_mouse_key_set;
    
    errcode_t ret = sle_low_latency_mouse_register_callbacks(&mouse_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("sle_low_latency_mouse_app_init: register mouse callbacks failed, ret=0x%x\n", ret);
    } else {
        osal_printk("sle_low_latency_mouse_app_init: mouse callbacks registered successfully\n");
    }
    
    // 同时注册通用的低延迟回调（用于EM数据控制）
    sle_low_latency_callbacks_t cbks = {0};
    cbks.hid_data_cb = NULL;  // mouse模式不需要hid_data_cb，使用专用的mouse回调
    cbks.sle_set_em_data_cb = sle_mouse_set_em_data_cbk;
    
    ret = sle_low_latency_register_callbacks(&cbks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("sle_low_latency_mouse_app_init: register general callbacks failed, ret=0x%x\n", ret);
    } else {
        osal_printk("sle_low_latency_mouse_app_init: general callbacks registered successfully\n");
    }
    
    return;
}

/**
 * @brief Dongle回调，获取鼠标数据并上报USB
 * @param[out] data 数据指针
 * @param[out] length 数据长度
 * @param[out] device_index 设备索引
 */
void dongle_cbk(uint8_t **data, uint16_t *length, uint8_t *device_index)
{
    static uint8_t report_count = 0;
    static uint32_t total_call_count = 0;
    
    total_call_count++;
    
    // 添加调试：定期打印调用状态
    if (total_call_count % 5000 == 0) {
        osal_printk("dongle_cbk: called %d times, report_time=%d, data_ready=%d\n", 
                   total_call_count, g_report_time, g_dongle_data_ready);
    }
    
    if (report_count < g_report_time) {
        report_count++;
        return;
    }
    report_count = 0;

    static usb_hid_mouse_report_t mouse_message = {0};
    low_latency_mouse_t key_base = {0};

    // 检查是否有新的低延迟数据
    if (!g_dongle_data_ready) {
        // 减少 debug 输出频率
        if (total_call_count % 10000 == 0) {
            osal_printk("dongle_cbk: no data ready (call #%d)\n", total_call_count);
        }
        return;
    }

    osal_printk("dongle_cbk: processing data (call #%d)\n", total_call_count);

    // 检查数据长度是否正确
    if (sizeof(g_dongle_data_buffer) < sizeof(low_latency_mouse_t)) {
        osal_printk("dongle_cbk: buffer size too small\n");
        return;
    }

    // 从缓存中读取数据
    if (memcpy_s(&key_base, sizeof(key_base), g_dongle_data_buffer, sizeof(key_base)) != EOK) {
        osal_printk("dongle_cbk: memcpy from buffer failed\n");
        return;
    }
    
    g_dongle_data_ready = false;  // 数据已消费，清除标志

    // 添加调试：打印接收到的原始数据
    osal_printk("dongle_cbk: raw data - button=0x%02x, x=%d, y=%d, wheel=%d\n", key_base.button_mask, key_base.x,
                key_base.y, key_base.wheel);

    mouse_message.key.d8 = key_base.button_mask;
    mouse_message.x = key_base.x;
    mouse_message.y = key_base.y;
    mouse_message.wheel = key_base.wheel;
    mouse_message.kind = MOUSE_KIND;

    // 添加调试：打印最终USB报告
    if (key_base.button_mask != 0 || key_base.x != 0 || key_base.y != 0 || key_base.wheel != 0) {
        osal_printk("dongle_cbk: USB report - kind=0x%02x, key=0x%02x, x=%d, y=%d, wheel=%d\n", mouse_message.kind,
                    mouse_message.key.d8, mouse_message.x, mouse_message.y, mouse_message.wheel);
    }

    *data = (uint8_t *)&mouse_message;
    *length = sizeof(usb_hid_mouse_report_t);
    *device_index = g_usb_mouse_hid_index;

#ifdef DEBUG
    // 添加调试：确认数据已设置
    osal_printk("dongle_cbk: data set, length=%d, device_index=%d\n", *length, *device_index);
#endif
}

/**
 * @brief USB高精度鼠标数据上报
 * @param[in] data 鼠标数据
 * @param[in] lenth 数据长度
 */
void usb_sle_high_mouse_report(uint8_t *data, uint8_t lenth)
{
    if (lenth > sizeof(g_send_mouse_msg)) {
        osal_printk("Invalide data\r\n");
        return;
    }
    g_send_mouse_msg.key.d8 = data[0];
    int16_t temp_x = data[1] | ((data[DATA_BIT2] & 0xf) << DATA_BIT8);
    int16_t temp_y = (data[DATA_BIT3] << DATA_BIT4) | ((data[DATA_BIT2] & 0xf0) >> DATA_BIT4);
    g_send_mouse_msg.x = trans_to_16_bit((uint16_t)temp_x, BLE_HID_MOUSE_HIGH_XY_BIT);
    g_send_mouse_msg.y = trans_to_16_bit((uint16_t)temp_y, BLE_HID_MOUSE_HIGH_XY_BIT);
    g_send_mouse_msg.wheel = (int8_t)data[WHEEL_DATA];
    g_send_mouse_msg.kind = MOUSE_KIND;
    fhid_send_data(g_usb_mouse_hid_index, (char *)&g_send_mouse_msg, USB_MOUSE_REPORTER_LEN);
}

/**
 * @brief Sle低延迟dongle初始化，注册USB回调和低延迟回调
 * @param[in] usb_hid_index USB HID索引
 */
void sle_low_latency_dongle_init(int usb_hid_index)
{
    g_usb_mouse_hid_index = usb_hid_index;
    
    osal_printk("sle_low_latency_dongle_init: starting initialization...\n");
    
    // 注册USB SOF回调
    usb_register_callback(&dongle_cbk);
    osal_printk("sle_low_latency_dongle_init: USB SOF callback registered\n");
    
    // 注册SLE低延迟Dongle回调
    sle_low_latency_dongle_callbacks_t dongle_cbk_struct = {0};
    dongle_cbk_struct.report_cb = dongle_low_latency_report_callback;
    
    errcode_t ret = sle_low_latency_dongle_register_callbacks(&dongle_cbk_struct);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("sle_low_latency_dongle_init: register callbacks failed, ret=0x%x\n", ret);
        return;
    }
    osal_printk("sle_low_latency_dongle_init: SLE low latency callbacks registered\n");
    
    // 使能SLE低延迟Dongle
    ret = sle_low_latency_dongle_enable();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("sle_low_latency_dongle_init: enable dongle failed, ret=0x%x\n", ret);
        return;
    }
    osal_printk("sle_low_latency_dongle_init: SLE low latency dongle enabled\n");
    
    osal_printk("sle_low_latency_dongle_init: initialized successfully\n");
}
