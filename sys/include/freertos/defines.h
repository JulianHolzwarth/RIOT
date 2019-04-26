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
 * @brief       general defines for freertos
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 * copied from cpu/esp32/include/esp_common.h and
 * copied from cpu/esp32/include/irq_arch.h
 * needed for it to work
 *
 * @author      Gunar Schorcht
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#ifndef FREERTOS_DEFINES_H
#define FREERTOS_DEFINES_H

#ifndef DOXYGEN

#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   microseconds per millisecond
 *
 */
#ifndef USEC_PER_MSEC
#define USEC_PER_MSEC 1000UL
#endif

/**
 * @brief   millisecond per second
 *
 */
#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC  1000UL
#endif

/**
 * @brief   the primary cpu
 *
 * the freertos implementation only supports this cpu
 *
 */
#ifndef PRO_CPU_NUM
#define PRO_CPU_NUM (0)
#endif


#if ENABLE_DEBUG
/**
 * @brief  Parameter check with return a value.
 *
 * If ENABLE_DEBUG is true, the macro checks a condition and returns with a value
 * if the condition is not fulfilled.
 * @param  cond    the condition
 * @param  err     the return value in the case the condition is not fulfilled.
 */

#ifndef CHECK_PARAM_RET
#define CHECK_PARAM_RET(cond, err)   if (!(cond)) \
    { \
        DEBUG("%s parameter condition (" #cond ") " \
              "not fulfilled\n", __func__); \
        return err; \
    }
#endif

/**
 * @brief  Parameter check without return value.
 *
 * If ENABLE_DEBUG is true, the macro checks a condition and returns without a
 * value if the condition is not fulfilled.
 * @param  cond    the condition
 */
#ifndef CHECK_PARAM
#define CHECK_PARAM(cond)   if (!(cond)) \
    { \
        DEBUG("%s parameter condition (" #cond ") " \
              "not fulfilled\n", __func__); \
        return; \
    }
#endif

#else /* ENABLE_DEBUG */

#ifndef CHECK_PARAM_RET
#define CHECK_PARAM_RET(cond, err) if (!(cond)) { return err; }
#endif
#ifndef CHECK_PARAM
#define CHECK_PARAM(cond)         if (!(cond)) { return; }
#endif

#endif /* ENABLE_DEBUG */

/* copied from cpu/esp32/include/irq_arch.h */

/**
 * @brief   Macros to enter and exit from critical region
 *
 * NOTE: since they use a local variable they can be used only in same function
 * @{
 */

#define critical_enter()   int _irq_state = irq_disable()

#define critical_exit()    irq_restore(_irq_state)


#ifdef __cplusplus
}
#endif

#endif  /* DOXYGEN */

#endif  /* FREERTOS_DEFINES_H */
