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
 * @brief       Implementation of freertos timers
 * 
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 * Implementation of freertos timers. xBlockTime does not get used in
 * any function.
 *
 * @author      Gunar Schorcht
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#ifndef FREERTOS_TIMERS_H
#define FREERTOS_TIMERS_H

#ifndef DOXYGEN

#include "freertos/FreeRTOS.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* TimerHandle_t;

#define TimerCallbackFunction_t  xtimer_callback_t

TimerHandle_t xTimerCreate (const char * const pcTimerName,
                            const TickType_t xTimerPeriod,
                            const UBaseType_t uxAutoReload,
                            void * const pvTimerID,
                            TimerCallbackFunction_t pxCallbackFunction);
BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xBlockTime);
BaseType_t xTimerStart (TimerHandle_t xTimer, TickType_t xBlockTime);
BaseType_t xTimerStop  (TimerHandle_t xTimer, TickType_t xBlockTime);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* FREERTOS_TIMERS_H */
