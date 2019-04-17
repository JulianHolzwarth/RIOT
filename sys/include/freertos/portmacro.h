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
 * @brief       freertos portmacro
 * 
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 * makro defines for critical sections and ports 
 *
 * @author      Gunar Schorcht
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#ifndef FREERTOS_PORTMACRO_H
#define FREERTOS_PORTMACRO_H

#ifndef DOXYGEN

#include "stdint.h"

#include "mutex.h"
#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

#define portBASE_TYPE                   int32_t
#define portUBASE_TYPE                  uint32_t

#define portMAX_DELAY                   0xFFFFFFFF

#define portMUX_TYPE                    mutex_t
#define portMUX_INITIALIZER_UNLOCKED    MUTEX_INIT

#define portENTER_CRITICAL(pm)          mutex_lock(pm)
#define portEXIT_CRITICAL(pm)           mutex_unlock(pm)
#define portENTER_CRITICAL_NESTED       irq_disable
#define portEXIT_CRITICAL_NESTED        irq_restore

#define portENTER_CRITICAL_ISR(mux)     vTaskEnterCritical(mux)
#define portEXIT_CRITICAL_ISR(mux)      vTaskExitCritical(mux)

#define taskENTER_CRITICAL(mux)         portENTER_CRITICAL(mux)
#define taskENTER_CRITICAL_ISR(mux)        portENTER_CRITICAL_ISR(mux)
#define taskEXIT_CRITICAL(mux)          portEXIT_CRITICAL(mux)
#define taskEXIT_CRITICAL_ISR(mux)        portEXIT_CRITICAL_ISR(mux)

#define portYIELD_FROM_ISR              thread_yield_higher
#define portNUM_PROCESSORS              2

#define xPortGetCoreID()                PRO_CPU_NUM

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* FREERTOS_PORTMACRO_H */
