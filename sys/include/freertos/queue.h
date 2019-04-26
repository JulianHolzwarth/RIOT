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
 * @brief       Implementation of freertos queue and freertos counting semaphore
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 * Please use the freertos function names, not the generic functions.
 *
 * @author      Gunar Schorcht
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#ifndef FREERTOS_QUEUE_H
#define FREERTOS_QUEUE_H

#ifndef DOXYGEN

#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#define xQueueHandle     QueueHandle_t

typedef void *QueueHandle_t;

/**
 * @brief   creates a generic queue
 *
 * @param   uxQueueLength   lenght of the queue in items
 * @param   uxItemSize      size of one item in bytes
 * @param   ucQueueType     type of the queue
 *
 * @return  the queue handle
 * @return  NULL if not successful
 */
QueueHandle_t xQueueGenericCreate(const UBaseType_t uxQueueLength,
                                  const UBaseType_t uxItemSize,
                                  const uint8_t ucQueueType);

/**
 * @brief   creates a counting semaphore as a queue
 *
 * @param   uxMaxCount      the maximal count of the semaphore
 * @param   uxInitialCount  the count of available spots
 *                          at creation of the semaphore
 *
 *
 * @return  the queue handle
 * @return  NULL if not successful
 */
QueueHandle_t xQueueCreateCountingSemaphore(const UBaseType_t uxMaxCount,
                                            const UBaseType_t uxInitialCount);

/**
 * @brief   deletes a queue
 *
 * @param   xQueue the handle of the Queue
 *
 */
void vQueueDelete(QueueHandle_t xQueue);

/**
 * @brief   resets a queue
 * not implemented
 */
BaseType_t xQueueGenericReset(QueueHandle_t xQueue, BaseType_t xNewQueue);

/**
 * @brief   receive for queue
 *
 * @param   xQueue          the handle of the Queue
 * @param   pvBuffer        the pointer to the buffer to put in the item
 * @param   xTicksToWait    amount of time to wait for a item in the queue
 * @param   xJustPeeking    if true does not remove the item
 *
 * @return  pdPass
 * @return  errQUEUE_EMPTY if not successful
 */
BaseType_t xQueueGenericReceive(QueueHandle_t xQueue,
                                void *const pvBuffer,
                                TickType_t xTicksToWait,
                                const BaseType_t xJustPeeking);

/**
 * @brief   send for queue
 *
 * @param   xQueue          the handle of the Queue
 * @param   pvItemToQueue   pointer to the item to be send
 * @param   xTicksToWait    amount of time to wait for space in the queue
 * @param   xCopyPosition   position to copy the item
 *                          use: queueSEND_TO_BACK, queueSEND_TO_FRONT,
 *                          queueOVERWRITE
 *
 * @return  pdPass          if successful
 * @return  errQUEUE_FULL   if no space in the queue
 * @return  pdFail          if xQueue is NULL
 */
BaseType_t xQueueGenericSend(QueueHandle_t xQueue,
                             const void *const pvItemToQueue,
                             TickType_t xTicksToWait,
                             const BaseType_t xCopyPosition);

/**
 * @brief   receive in ISR for queue
 *
 * @param   xQueue                      the handle of the Queue
 * @param   pvBuffer
 * @param   pxHigherPriorityTaskWoken   pointer to value
 *                                      if True the thread with higher
 *                                      priority woke up after this function
 *
 * @return pdPass           if successful
 * @return errQUEUE_EMPTY   if the queue was empty
 * @return  pdFail           if xQueue is NULL
 */
BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void *const pvBuffer,
                                BaseType_t *const pxHigherPriorityTaskWoken);

/**
 * @brief   send in ISR for queue
 *
 * @param   xQueue                      the handle of the Queue
 * @param   pvItemToQueue               pointer to the item to be send
 * @param   pxHigherPriorityTaskWoken   pointer to value
 *                                      if True the thread with higher
 *                                      priority woke up after this function
 *
 * @param   xCopyPosition               position to copy the item
 *                                      use: queueSEND_TO_BACK, queueSEND_TO_FRONT,
 *                                      queueOVERWRITE
 *
 * @return  pdPass          if successful
 * @return  errQUEUE_FULL   if no space in the queue
 * @return  pdFail          if xQueue is NULL
 */
BaseType_t xQueueGenericSendFromISR(QueueHandle_t xQueue,
                                    const void *const pvItemToQueue,
                                    BaseType_t *const pxHigherPriorityTaskWoken,
                                    const BaseType_t xCopyPosition );

/**
 * @brief   for xSemaphoreGiveFromISR in semphr.h
 *
 * not implemented
 *
 * @return pdFail
 */
BaseType_t xQueueGiveFromISR(QueueHandle_t xQueue,
                             BaseType_t *const pxHigherPriorityTaskWoken);

/**
 * @brief   waiting messages
 *
 * @param       xQueue   the handle of the queue
 *
 * @return number of items held by xQueue
 */
UBaseType_t uxQueueMessagesWaiting( QueueHandle_t xQueue );

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

#define queueSEND_TO_BACK         (( BaseType_t )0)
#define queueSEND_TO_FRONT        (( BaseType_t )1)
#define queueOVERWRITE            (( BaseType_t )2)

#define queueQUEUE_TYPE_BASE                (( uint8_t )0U)
#define queueQUEUE_TYPE_SET                 (( uint8_t )0U)
#define queueQUEUE_TYPE_MUTEX               (( uint8_t )1U)
#define queueQUEUE_TYPE_COUNTING_SEMAPHORE  (( uint8_t )2U)
#define queueQUEUE_TYPE_BINARY_SEMAPHORE    (( uint8_t )3U)
#define queueQUEUE_TYPE_RECURSIVE_MUTEX     (( uint8_t )4U)

#define errQUEUE_EMPTY      (( BaseType_t )0)
#define errQUEUE_FULL       (( BaseType_t )0)
#define errQUEUE_BLOCKED    (-4)
#define errQUEUE_YIELD      (-5)

#define xQueueCreate( uxQueueLength, uxItemSize ) \
    xQueueGenericCreate((uxQueueLength), (uxItemSize), (queueQUEUE_TYPE_BASE))

#define xQueueReceive( xQueue, pvBuffer, xTicksToWait ) \
    xQueueGenericReceive((xQueue), (pvBuffer), (xTicksToWait), \
                         pdFALSE )

#define xQueueSend( xQueue, pvItemToQueue, xTicksToWait ) \
    xQueueGenericSend((xQueue), (pvItemToQueue), (xTicksToWait), \
                      queueSEND_TO_BACK )

#define xQueueSendToBack( xQueue, pvItemToQueue, xTicksToWait ) \
    xQueueGenericSend((xQueue), (pvItemToQueue), (xTicksToWait), \
                      queueSEND_TO_BACK )

#define xQueueSendFromISR( xQueue, pvItemToQueue, pxHigherPriorityTaskWoken ) \
    xQueueGenericSendFromISR((xQueue), (pvItemToQueue), \
                             (pxHigherPriorityTaskWoken), \
                             queueSEND_TO_BACK )

#define xQueueReset( xQueue ) xQueueGenericReset( xQueue, pdFALSE )

#ifdef __cplusplus
}
#endif

#endif  /* DOXYGEN */
#endif  /* FREERTOS_QUEUE_H */
