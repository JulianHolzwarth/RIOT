/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2019 Freie Universitaet Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#ifndef DOXYGEN

#define ENABLE_DEBUG 0
#include "debug.h"

#include <string.h>
#include "syscalls.h"

#include "freertos/task_common.h"
#include "freertos/task.h"
#include "log.h"
#include "thread.h"

#include "freertos/FreeRTOS.h"
#include "freertos/defines.h"



TickType_t xTaskGetTickCount(void)
{
    return system_get_time() / USEC_PER_MSEC / portTICK_PERIOD_MS;
}

BaseType_t xTaskCreatePinnedToCore(TaskFunction_t pvTaskCode,
                                   const char *const pcName,
                                   const uint32_t usStackDepth,
                                   void *const pvParameters,
                                   UBaseType_t uxPriority,
                                   TaskHandle_t *const pxCreatedTask,
                                   const BaseType_t xCoreID)
{

    /*unused variable*/
    (void)xCoreID;
    return xTaskCreate(pvTaskCode, pcName, usStackDepth, pvParameters,
                       uxPriority, pxCreatedTask);
}

#endif /* DOXYGEN */
