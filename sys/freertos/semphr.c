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

#include "log.h"
#include "mutex.h"
#include "rmutex.h"

#include "freertos/FreeRTOS.h"
#include "freertos/defines.h"

/*
 * In FreeRTOS different types of semaphores, mutexes and queues are all
 * mapped to a single generic queue type. With all these different types,
 * single functions for send, receive, give and take are then used. To be
 * able to dsitinguish between these different types in RIOT, we need typed
 * objects.
 */
typedef struct {
    uint8_t type;           /* type of the mutex, MUST be the first element */
    mutex_t mutex;          /* the mutex */
} _mutex_t;

typedef struct {
    uint8_t type;           /* type of the mutex, MUST be the first element */
    rmutex_t rmutex;        /* the mutex */
} _rmutex_t;

SemaphoreHandle_t xSemaphoreCreateMutex(void)
{
    _mutex_t *_tmp = (_mutex_t *)malloc(sizeof(_mutex_t));

    _tmp->type = queueQUEUE_TYPE_MUTEX;
    mutex_init(&_tmp->mutex);

    DEBUG("%s mutex=%p\n", __func__, (void *)_tmp);
    return _tmp;
}

void vSemaphoreDelete( SemaphoreHandle_t xSemaphore )
{
    DEBUG("%s mutex=%p\n", __func__, (void *)xSemaphore);

    CHECK_PARAM(xSemaphore != NULL);
    free(xSemaphore);
}

BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore)
{
    DEBUG("%s mutex=%p\n", __func__, (void *)xSemaphore);

    CHECK_PARAM_RET(xSemaphore != NULL, pdFALSE);

    uint8_t type = ((_mutex_t *)xSemaphore)->type;
    mutex_t *mutex = &((_mutex_t *)xSemaphore)->mutex;

    switch (type) {
        case queueQUEUE_TYPE_MUTEX:
            mutex_unlock(mutex);
            break;
        case queueQUEUE_TYPE_RECURSIVE_MUTEX:
            return xSemaphoreGiveRecursive(xSemaphore);
        default:
            return xQueueGenericSend(xSemaphore, NULL, 0, queueSEND_TO_BACK);
    }

    return pdTRUE;
}

BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore,
                          TickType_t xTicksToWait)
{
    DEBUG("%s mutex=%p wait=%u\n", __func__, (void *)xSemaphore, xTicksToWait);

    CHECK_PARAM_RET(xSemaphore != NULL, pdFAIL);

    uint8_t type = ((_mutex_t *)xSemaphore)->type;
    mutex_t *mutex = &((_mutex_t *)xSemaphore)->mutex;

    switch (type) {
        case queueQUEUE_TYPE_MUTEX:
        {
            if (xTicksToWait == 0) {
                return (mutex_trylock(mutex) == 1) ? pdPASS : pdFAIL;
            }
            else if (xTicksToWait == portMAX_DELAY) {
                mutex_lock(mutex);
                return pdPASS;
            }
            else {
                assert(0);
                /* TODO timeout handling */
                return pdFAIL;
            }
            break;
        }
        case queueQUEUE_TYPE_RECURSIVE_MUTEX:
            return xSemaphoreTakeRecursive(xSemaphore, xTicksToWait);

        default:
            return xQueueGenericReceive(xSemaphore, NULL, xTicksToWait,
                                        pdFALSE);
    }
}

UBaseType_t uxSemaphoreGetCount( SemaphoreHandle_t xSemaphore )
{
    CHECK_PARAM_RET(xSemaphore != NULL, pdFALSE);
    mutex_t *mutex;
    rmutex_t *rmutex;

    uint8_t type = ((_mutex_t *)xSemaphore)->type;
    switch (type) {
        case queueQUEUE_TYPE_MUTEX:
            mutex = &((_mutex_t *)xSemaphore)->mutex;
            if (mutex->queue.next == NULL) {
                return 1;
            }
            else {
                return 0;
            }
            break;
        case queueQUEUE_TYPE_RECURSIVE_MUTEX:
            rmutex = &((_rmutex_t *)xSemaphore)->rmutex;
            if (rmutex->refcount == 0) {
                return 1;
            }
            else {
                return 0;
            }
        default:
            return uxQueueMessagesWaiting(xSemaphore);
    }
}

SemaphoreHandle_t xSemaphoreCreateRecursiveMutex(void)
{
    _rmutex_t *_tmp = (_rmutex_t *)malloc(sizeof(_rmutex_t));

    _tmp->type = queueQUEUE_TYPE_RECURSIVE_MUTEX;
    rmutex_init(&_tmp->rmutex);

    DEBUG("%s rmutex=%p\n", __func__, (void *)_tmp);

    return _tmp;
}

BaseType_t xSemaphoreGiveRecursive(SemaphoreHandle_t xSemaphore)
{
    DEBUG("%s rmutex=%p\n", __func__, (void *)xSemaphore);

    CHECK_PARAM_RET(xSemaphore != NULL, pdFALSE);
    CHECK_PARAM_RET(((_rmutex_t *)xSemaphore)->type ==
                    queueQUEUE_TYPE_RECURSIVE_MUTEX, pdFALSE);

    rmutex_unlock(&((_rmutex_t *)xSemaphore)->rmutex);
    return pdTRUE;
}

BaseType_t xSemaphoreTakeRecursive(SemaphoreHandle_t xSemaphore,
                                   TickType_t xTicksToWait)
{
    DEBUG("%s rmutex=%p wait=%u\n", __func__, (void *)xSemaphore, xTicksToWait);

    CHECK_PARAM_RET(xSemaphore != NULL, pdFALSE);
    CHECK_PARAM_RET(((_rmutex_t *)xSemaphore)->type ==
                    queueQUEUE_TYPE_RECURSIVE_MUTEX, pdFALSE);

    BaseType_t ret = pdTRUE;
    rmutex_t *rmutex = &((_rmutex_t *)xSemaphore)->rmutex;

    if (xTicksToWait == 0) {
        ret = (rmutex_trylock(rmutex) == 1) ? pdPASS : pdFAIL;
    }
    else if (xTicksToWait == portMAX_DELAY) {
        rmutex_lock(&((_rmutex_t *)xSemaphore)->rmutex);
    }
    else {
        ret = pdFAIL;
        assert(0);
        /* TODO timeout handling */
    }

    return ret;
}

void vPortCPUAcquireMutex(portMUX_TYPE *mux)
{
    DEBUG("%s pid=%d prio=%d mux=%p\n", __func__,
          thread_getpid(), sched_threads[thread_getpid()]->priority,
          (void *)mux);
    critical_enter();
    mutex_lock(mux);     /* lock the mutex with interrupts disabled */
    critical_exit();
}

void vPortCPUReleaseMutex(portMUX_TYPE *mux)
{
    DEBUG("%s pid=%d prio=%d mux=%p\n", __func__,
          thread_getpid(), sched_threads[thread_getpid()]->priority,
          (void *)mux);
    critical_enter();
    mutex_unlock(mux);     /* unlock the mutex with interrupts disabled */
    critical_exit();
}

#endif /* DOXYGEN */
