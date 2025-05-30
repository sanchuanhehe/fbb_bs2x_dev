/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: diag set log lvl
 */

#ifndef DIAG_CMD_SET_LOG_LEVEL_H
#define DIAG_CMD_SET_LOG_LEVEL_H

#include "errcode.h"
#include "diag.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    uint32_t core;
    uint32_t log_lvl;
} diag_log_lvl_data_t;

errcode_t diag_cmd_set_log_level(uint16_t cmd_id, uint8_t *cmd_param, uint16_t cmd_param_size,
    diag_option_t *option);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif // !DIAG_CMD_SET_LOG_LEVEL_H