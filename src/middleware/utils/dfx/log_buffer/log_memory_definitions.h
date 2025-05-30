/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: LOG MEMORY REGION DEFINITIONS.
 *
 * Create:  2018-10-15
 */

#ifndef NON_OS_LOG_MEMORY_DEFINITIONS_H
#define NON_OS_LOG_MEMORY_DEFINITIONS_H

#include "memory_config.h"
#include "stdint.h"
#include "chip_definitions.h"
#include "platform_core.h"
/**
 * @addtogroup connectivity_drivers_non_os_log
 * @{
 */
/**
 * @brief  LOG MEMORY REGIONS
 */
typedef enum {
    LOG_MEMORY_REGION_SECTION_0 = 0,  // !< LOG_MEMORY_REGION_SECTION_0 for app core
#if CORE_NUMS > 1
    LOG_MEMORY_REGION_SECTION_1,      // !< LOG_MEMORY_REGION_SECTION_1 for application core
#endif
#if CORE_NUMS > 2
    LOG_MEMORY_REGION_SECTION_2,      // !< LOG_MEMORY_REGION_SECTION_2 for hifi core when dsp exists.
#endif                                // !< Otherwise, for gnss core.
#if GNSS_EXIST == YES
    LOG_MEMORY_REGION_SECTION_3,      // !< LOG_MEMORY_REGION_SECTION_3 for gnss core
#endif
#if SECURITY_EXIST == YES
    LOG_MEMORY_REGION_SECTION_4,      // !< LOG_MEMORY_REGION_SECTION_4 for sec core
#endif
    LOG_MEMORY_REGION_MAX_NUMBER,     // !< LOG_MEMORY_REGION_MAX_NUMBER
} log_memory_region_section_t;

/**
 * @brief  Log memory control structure for every section
 */
typedef struct {
    // reader controlled
    volatile uint32_t read;

    // writer controlled
    volatile uint32_t write;
} log_memory_section_control_t;

/**
 * @brief  Log memory area control structure
 */
typedef struct {
    uint32_t offset[LOG_MEMORY_REGION_MAX_NUMBER];
    uint32_t length;
    log_memory_section_control_t section_control[LOG_MEMORY_REGION_MAX_NUMBER];
} log_memory_region_control_t;

/** Macro to get the pointer to the log memory control block */
#define LOG_MEMORY_REGION_CONTROL_BLOCK_POINTER ((const log_memory_region_control_t *)(uintptr_t)LOGGING_REGION_START)

/**
 * @}
 */
#endif
