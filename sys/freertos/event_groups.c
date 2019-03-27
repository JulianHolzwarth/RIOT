/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#ifndef DOXYGEN

#define ENABLE_DEBUG (0)
#include "debug.h"

#include <string.h>


#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

EventGroupHandle_t xEventGroupCreate (void)
{
    DEBUG_PRINT("%s\n", __func__);
    return NULL;
}

void vEventGroupDelete (EventGroupHandle_t xEventGroup)
{
    (void) xEventGroup;
    DEBUG_PRINT("%s\n", __func__);
    return;
}

EventBits_t xEventGroupSetBits (EventGroupHandle_t xEventGroup,
                                const EventBits_t uxBitsToSet)
{
    (void) xEventGroup;
    (void) uxBitsToSet;
    DEBUG_PRINT("%s\n", __func__);
    return (EventBits_t) NULL;
}

EventBits_t xEventGroupClearBits (EventGroupHandle_t xEventGroup,
                                  const EventBits_t uxBitsToClear )
{
    (void) xEventGroup;
    (void) uxBitsToClear;
    DEBUG_PRINT("%s\n", __func__);
    return (EventBits_t) NULL;
}

EventBits_t xEventGroupWaitBits (const EventGroupHandle_t xEventGroup,
                                 const EventBits_t uxBitsToWaitFor,
                                 const BaseType_t xClearOnExit,
                                 const BaseType_t xWaitForAllBits,
                                 TickType_t xTicksToWait)
{
    (void) xEventGroup;
    (void) uxBitsToWaitFor;
    (void) xClearOnExit;
    (void) xWaitForAllBits;
    (void) xTicksToWait;
    DEBUG_PRINT("%s\n", __func__);
    return (EventBits_t) NULL;
}

#endif /* DOXYGEN */
