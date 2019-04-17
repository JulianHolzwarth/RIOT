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
 * @brief       freertos defines
 * 
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 *
 * @author      Gunar Schorcht
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#ifndef FREERTOS_FREERTOS_H
#define FREERTOS_FREERTOS_H

#ifndef DOXYGEN

#include "freertos/portmacro.h"

#ifdef __cplusplus
extern "C" {
#endif

#define configMAX_PRIORITIES    SCHED_PRIO_LEVELS

#ifndef configASSERT
#define configASSERT    assert
#endif

#ifndef portTICK_PERIOD_MS
#define portTICK_PERIOD_MS      10
#endif

#ifndef MHZ
#define MHZ 1000000
#endif
#define portTickType            TickType_t

typedef int32_t  BaseType_t;
typedef uint32_t UBaseType_t;
typedef uint32_t TickType_t;

/**
 * @brief   return the freertos tickrate
 *
 * @return tickrate
 */
uint32_t xPortGetTickRateHz(void);

/**
 * @brief   returns the isr context
 *
 * @return true, if in isr context, false if not
 */
BaseType_t xPortInIsrContext(void);

/*
 * PLESE NOTE: Following definitions were copied directly from the FreeRTOS
 * distribution and are under the following copyright:
 *
 * FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
 * All rights reserved
 *
 * FreeRTOS is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License (version 2) as published by the
 * Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.
 *
 * Full license text is available on the following
 * link: http://www.freertos.org/a00114.html
 */

#define pdFALSE  ( ( BaseType_t ) 0 )
#define pdTRUE   ( ( BaseType_t ) 1 )
#define pdPASS   ( pdTRUE )
#define pdFAIL   ( pdFALSE )

#ifdef __cplusplus
}
#endif

#include "freertos/semphr.h"
#include "freertos/queue.h"

#endif /* DOXYGEN */
#endif /* FREERTOS_FREERTOS_H */
