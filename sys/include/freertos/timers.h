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

typedef void *TimerHandle_t;

#define TimerCallbackFunction_t  xtimer_callback_t

/**
 * @brief    creates a timer
 *
 * @param   pcTimerName         the name of the timer (for debugging)
 * @param   xTimerPeriod        period the timer runs in ticks
 * @param   uxAutoReload        pdTrue: automatically reloading the timer
 *                              else oneshot timer
 * @param   pvTimerID           can be used to store values between calls
 *                              and to determine which timer expired when
 *                              multiple timers have the same callback function
 * @param   pxCallbackFunction  the callback function
 *                              called when the timer expires
 *
 * @return  timer handle    if successful
 * @return  NULL            if no memory for the timer
 */
TimerHandle_t xTimerCreate(const char *const pcTimerName,
                           const TickType_t xTimerPeriod,
                           const UBaseType_t uxAutoReload,
                           void *const pvTimerID,
                           TimerCallbackFunction_t pxCallbackFunction);

/**
 * @brief    deletes a timer
 *
 * @param   xTimer      the timer handle
 * @param   xBlockTime  not used
 *
 * @return  pdTRUE    if successful
 * @return  pdFALSE   else
 */
BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xBlockTime);

/**
 * @brief    starts a timer
 *
 * @param   xTimer      the timer handle
 * @param   xBlockTime  not used
 *
 * @return  pdTRUE    if successful
 * @return  pdFALSE   else
 */
BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t xBlockTime);

/**
 * @brief    stops a timer
 *
 * @param   xTimer      the timer handle
 * @param   xBlockTime  not used
 *
 * @return  pdTRUE    if successful
 * @return  pdFALSE   else
 */
BaseType_t xTimerStop(TimerHandle_t xTimer, TickType_t xBlockTime);

/**
 * @brief    gives the id of a timer
 *
 * @param   xTimer      the timer handle
 *
 * @return  the id    if successful
 * @return  pdFALSE   else
 */
void *pvTimerGetTimerID(TimerHandle_t xTimer);

/**
 * @brief    sets the id of a timer
 *
 * @param   xTimer      the timer handle
 */
void vTimerSetTimerID(TimerHandle_t xTimer, void *pvNewID);

#ifdef __cplusplus
}
#endif

#endif  /* DOXYGEN */
#endif  /* FREERTOS_TIMERS_H */
