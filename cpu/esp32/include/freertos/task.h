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
 * @ingroup     esp32_freertos
 * @{
 *
 * @file
 * @brief       Implementation of freertos tasks
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 * The rest of the implementation is in ys/include/freertos/task_common.h
 * This is file is needed because esp32 has functions only for it.
 *
 * @author      Gunar Schorcht
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#ifndef FREERTOS_TASK_H
#define FREERTOS_TASK_H

#ifndef DOXYGEN

#include "freertos/task_common.h"
#include "freertos/FreeRTOS.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

TickType_t xTaskGetTickCount(void);

BaseType_t xTaskCreatePinnedToCore(TaskFunction_t pvTaskCode,
                                   const char *const pcName,
                                   const uint32_t usStackDepth,
                                   void *const pvParameters,
                                   UBaseType_t uxPriority,
                                   TaskHandle_t *const pvCreatedTask,
                                   const BaseType_t xCoreID);

#ifdef __cplusplus
}
#endif

#endif  /* DOXYGEN */
#endif  /* FREERTOS_TASK_H */
