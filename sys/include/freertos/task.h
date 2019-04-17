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
 * Implemented is the creation of a freetos task, the delay and
 * delete. The scheduler is still riots.
 *
 * @author      Gunar Schorcht
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#ifndef FREERTOS_TASK_H
#define FREERTOS_TASK_H

#ifndef DOXYGEN

#include "thread.h"
#include "FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#define xTaskHandle     TaskHandle_t
#define tskNO_AFFINITY  INT_MAX
#define errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY pdFAIL

typedef void (*TaskFunction_t)(void *);

typedef void* TaskHandle_t;

BaseType_t xTaskCreate (TaskFunction_t pvTaskCode,
                        const char * const pcName,
                        const uint32_t usStackDepth,
                        void * const pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t * const pvCreatedTask);

BaseType_t xTaskCreatePinnedToCore (TaskFunction_t pvTaskCode,
                                    const char * const pcName,
                                    const uint32_t usStackDepth,
                                    void * const pvParameters,
                                    UBaseType_t uxPriority,
                                    TaskHandle_t * const pvCreatedTask,
                                    const BaseType_t xCoreID);

void vTaskDelete (TaskHandle_t xTaskToDelete);
void vTaskDelay (const TickType_t xTicksToDelay);

TaskHandle_t xTaskGetCurrentTaskHandle (void);

void vTaskEnterCritical (portMUX_TYPE *mux);
void vTaskExitCritical (portMUX_TYPE *mux);

TickType_t xTaskGetTickCount (void);
TickType_t portTICK_RATE_MS(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* FREERTOS_TASK_H */
