/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 HAL timer \n
 *
 * History: \n
 * 2022-12-08, Create file. \n
 */
#ifndef HAL_TIMER_V150_H
#define HAL_TIMER_V150_H

#include <stdint.h>
#include "hal_drv_timer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_timer_v150 TIMER V150
 * @ingroup  drivers_hal_timer
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of the timer mode.
 * @else
 * @brief  Timer模式定义。
 * @endif
 */
typedef enum control_reg_mode_v150 {
    /** @if Eng  Timer mode: one shot mode(Same as TIMER_V150_MODE_ONE_SHOT_1).
     *  @else    定时器控制模式：单触发模式（等同于TIMER_V150_MODE_ONE_SHOT_1）。
     *  @endif */
    TIMER_V150_MODE_ONE_SHOT,
    /** @if Eng  Timer mode: periodic mode.
     *  @else    定时器控制模式：周期触发模式。
     *  @endif */
    TIMER_V150_MODE_PERIODIC,
    /** @if Eng  Timer mode: one shot mode(Same as TIMER_V150_MODE_ONE_SHOT).
     *  @else    定时器控制模式：单触发模式（等同于TIMER_V150_MODE_ONE_SHOT）。
     *  @endif */
    TIMER_V150_MODE_ONE_SHOT_1,
    /** @if Eng  Timer mode: free run mode.
     *  @else    定时器控制模式：自由运行模式。
     *  @endif */
    TIMER_V150_MODE_FREERUN
} control_reg_mode_v150_t;

/**
 * @if Eng
 * @brief  HAL timer init interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  callback Callback of timer.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层Timer的初始化接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @param  [in]  callback Timer的回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_timer_v150_init(timer_index_t index, hal_timer_callback_t callback);

/**
 * @if Eng
 * @brief  HAL timer deinit interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  HAL层Timer的去初始化接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
void hal_timer_v150_deinit(timer_index_t index);

/**
 * @if Eng
 * @brief  HAL timer start the load count of hardware timer interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  HAL层启动硬件定时器计数的接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
void hal_timer_v150_start(timer_index_t index);

/**
 * @if Eng
 * @brief  HAL timer stop the load count of hardware timer interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  HAL层停止硬件定时器计数的接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
void hal_timer_v150_stop(timer_index_t index);

/**
 * @if Eng
 * @brief  HAL timer set the load count of hardware timer interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  delay_count Time for load count.
 * @param  [in]  mode mode of time.
 * @else
 * @brief  HAL层设置硬件计时器计数的接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @param  [in]  delay_count 计时的时间。
 * @param  [in]  mode 计时模式。
 * @endif
 */
void hal_timer_v150_config_load(timer_index_t index, uint64_t delay_count, uint32_t mode);

/**
 * @if Eng
 * @brief  HAL timer get the current value of hardware timer interface.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @param  [in]  is_enable Hardware timer enable status.
 * @return Timer load count.
 * @else
 * @brief  HAL层获取硬件当前计时器剩余计数的接口。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @param  [in]  is_enable 硬件定时器使能状态 。
 * @return Timer计数值。
 * @endif
 */
uint64_t hal_timer_v150_get_current_value(timer_index_t index, bool *is_enable);

/**
 * @if Eng
 * @brief  Handler of the timer interrupt request.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  timer中断处理函数。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
void hal_timer_v150_irq_handler(timer_index_t index);

/**
 * @if Eng
 * @brief  clear timer interrupt.
 * @param  [in]  index Index of the hardware timer. For detail, see @ref timer_index_t.
 * @else
 * @brief  清除timer模块内部的中断。
 * @param  [in]  index 硬件定时器索引值，参考 @ref timer_index_t 。
 * @endif
 */
void hal_timer_v150_interrupt_clear(timer_index_t index);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif