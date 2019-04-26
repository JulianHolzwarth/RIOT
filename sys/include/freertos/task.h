/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2019 Freie Universitaet Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_freertos
 * @{
 *
 * @file
 * @brief       Implementation of freertos tasks
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 * The rest is in task_common.h because esp32 needs a diffrent
 * xTaskGetTickCount() and has a esp32 spesific fuction called
 * xTaskCreatePinnedToCore().
 *
 * @author      Gunar Schorcht
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */
#ifdef CPU_ESP32
#include "../../../../cpu/esp32/include/freertos/task.h"
#else

#ifndef FREERTOS_TASK_H
#define FREERTOS_TASK_H

#ifndef DOXYGEN

#include "thread.h"
#include "FreeRTOS.h"
#include "task_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    returns tick count since start
 *
 * @return  tick count since start
 */
TickType_t xTaskGetTickCount(void);

#ifdef __cplusplus
}
#endif

#endif  /* DOXYGEN */
#endif  /* FREERTOS_TASK_H */

#endif  /* CPU_ESP32 */
