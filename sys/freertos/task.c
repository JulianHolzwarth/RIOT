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
#include "xtimer.h"

#include "freertos/defines.h"

TickType_t xTaskGetTickCount(void)
{
    return xtimer_now_usec64() / USEC_PER_MSEC / portTICK_PERIOD_MS;
}

#endif /* DOXYGEN */
