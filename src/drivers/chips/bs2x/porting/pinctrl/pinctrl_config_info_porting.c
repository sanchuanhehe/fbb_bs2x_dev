/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Provides pinctrl port \n
 *
 * History: \n
 * 2024-06-18, Create file. \n
 */
#include "pinctrl_porting.h"

const hal_pio_config_t g_pio_function_config[PIN_MAX_NUMBER] = {
#ifndef CONFIG_XO_32K_ENABLE
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO0
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO1
#else
    // LFXO1(Connection for 32.768 kHz crystal (LFXO))
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_NONE, PIN_IE_1 }, // !< S_MGPIO0
    // LFXO2(Connection for 32.768 kHz crystal (LFXO))
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_NONE, PIN_IE_1 }, // !< S_MGPIO1
#endif
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO2
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO3
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO4
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO5
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO6
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO7
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO8
#if defined(SUPPORT_CARKEY)
    { HAL_PIO_CAN_TX, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_UP, PIN_IE_1 }, // !< S_MGPIO9
    { HAL_PIO_CAN_RX, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_UP, PIN_IE_1 }, // !< S_MGPIO10
#else
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO9
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO10
#endif
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO11
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO12
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO13
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO14
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO15
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO16
#if defined(PRODUCT_AIR_MOUSE)
    { HAL_PIO_UART_L1_TXD, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_NONE, PIN_IE_1 }, // !< S_MGPIO17
    { HAL_PIO_UART_L1_RXD, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_MAX, PIN_IE_1 }, // !< S_MGPIO18
#else
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_NONE, PIN_IE_1 }, // !< S_MGPIO17
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO18
#endif
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_NONE, PIN_IE_1 }, // !< S_MGPIO19
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO20
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO21
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO22
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO23
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO24
#if defined(PRODUCT_AIR_MOUSE)
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_NONE, PIN_IE_1 }, // !< S_MGPIO25
#else
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO25
#endif
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO26
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO27
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO28
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO29
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO30
    { HAL_PIO_FUNC_GPIO, HAL_PIO_DRIVE_MAX, HAL_PIO_PULL_DOWN, PIN_IE_1 }, // !< S_MGPIO31
};
