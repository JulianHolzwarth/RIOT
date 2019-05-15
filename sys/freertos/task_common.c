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

#define ENABLE_DEBUG 0
#include "debug.h"

#include <string.h>

#include "log.h"
#include "thread.h"
#include "xtimer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task_common.h"
#include "freertos/defines.h"

/**
 * @brief   Architecture specific data of thread control blocks
 */
/**/
typedef struct {
    uint32_t saved_int_state;
    uint32_t critical_nesting;
} thread_arch_ext_t;

volatile thread_arch_ext_t threads_arch_exts[KERNEL_PID_LAST + 1];
// = {}

BaseType_t xTaskCreate(TaskFunction_t pvTaskCode,
                       const char *const pcName,
                       const uint32_t usStackDepth,
                       void *const pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t *const pxCreatedTask)
{
    /* FreeRTOS priority values have to be inverted */
    if (uxPriority >= configMAX_PRIORITIES) {
        uxPriority = 0;
    }
    else {
        uxPriority = THREAD_PRIORITY_MIN - uxPriority;
    }
    
    DEBUG("%s name=%s size=%d prio=%d pxCreatedTask=%p ",
          __func__, pcName, usStackDepth, uxPriority, (void *)pxCreatedTask);

    char *stack = malloc(usStackDepth + sizeof(thread_t));

    if (!stack) {
        return errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
    }
    kernel_pid_t pid = thread_create(stack,
                                     usStackDepth + sizeof(thread_t),
                                     uxPriority,
                                     THREAD_CREATE_WOUT_YIELD,
                                     (thread_task_func_t)pvTaskCode,
                                     pvParameters, pcName);
    DEBUG("pid=%d\n", pid);

    if (pxCreatedTask) {
        *pxCreatedTask = (TaskHandle_t)(0L + pid);
    }

    return (pid < 0) ? errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY : pdTRUE;
}

TaskHandle_t xTaskCreateStatic(TaskFunction_t pvTaskCode,
                               const char *const pcName,
                               uint32_t ulStackDepth,
                               void *pvParameters,
                               UBaseType_t uxPriority,
                               StackType_t *const puxStackBuffer,
                               StaticTask_t *const pxTaskBuffer)
{

    (void)pxTaskBuffer;

    /* FreeRTOS priority values have to be inverted */
    if (uxPriority >= configMAX_PRIORITIES) {
        uxPriority = 0;
    }
    else {
        uxPriority = THREAD_PRIORITY_MIN - uxPriority;
    }

    DEBUG("%s name=%s size=%d prio=%d",
          __func__, pcName, ulStackDepth, uxPriority);

    if (!puxStackBuffer) {
        return NULL;
    }

    kernel_pid_t pid = thread_create((char *)puxStackBuffer,
                                     ulStackDepth,
                                     uxPriority,
                                     THREAD_CREATE_WOUT_YIELD,
                                     (thread_task_func_t)pvTaskCode,
                                     pvParameters, pcName);
    DEBUG("pid=%d\n", pid);

    TaskHandle_t *pxCreatedTask = (TaskHandle_t)(0L + pid);

    return (pid < 0) ? NULL : pxCreatedTask;

}


void vTaskDelete(TaskHandle_t xTaskToDelete)
{
    DEBUG("%s pid=%d task=%p\n", __func__,
          thread_getpid(), (void *)xTaskToDelete);

    TaskHandle_t calling_task = xTaskGetCurrentTaskHandle();

    /* delete of another thread is not supported in riot */
    assert(xTaskToDelete == NULL || xTaskToDelete == calling_task);

    if (xTaskToDelete == NULL || xTaskToDelete == calling_task) {
        sched_task_exit();
    }
}

TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
    DEBUG("%s pid=%d\n", __func__, thread_getpid());

    uint32_t pid = thread_getpid();
    return (TaskHandle_t)pid;
}

void vTaskDelay( const TickType_t xTicksToDelay )
{
    uint64_t us = xTicksToDelay * MHZ / xPortGetTickRateHz();

    xtimer_usleep(us);
}

TickType_t portTICK_RATE_MS(uint32_t ms)
{
    return ms * USEC_PER_MSEC * xPortGetTickRateHz() / MHZ;
}

void vTaskEnterCritical( portMUX_TYPE *mux )
{
    /* determine calling thread pid (can't fail) */
    kernel_pid_t my_pid = thread_getpid();

    DEBUG("%s pid=%d prio=%d mux=%p\n", __func__,
          my_pid, sched_threads[my_pid]->priority, (void *)mux);

    /* disable interrupts */
    uint32_t state = irq_disable();

    /* aquire the mutex with interrupts disabled */
    mutex_lock(mux); /* TODO should be only a spin lock */

    /* increment nesting counter and save old interrupt level */
    threads_arch_exts[my_pid].critical_nesting++;
    if (threads_arch_exts[my_pid].critical_nesting == 1) {
        threads_arch_exts[my_pid].saved_int_state = state;
    }
}

void vTaskExitCritical( portMUX_TYPE *mux )
{
    /* determine calling thread pid (can't fail) */
    kernel_pid_t my_pid = thread_getpid();

    DEBUG("%s pid=%d prio=%d mux=%p\n", __func__,
          my_pid, sched_threads[my_pid]->priority, (void *)mux);

    /* release the mutex with interrupts disabled */
    mutex_unlock(mux); /* TODO should be only a spin lock */

    /* decrement nesting counter and restore old interrupt level */
    if (threads_arch_exts[my_pid].critical_nesting) {
        threads_arch_exts[my_pid].critical_nesting--;
        if (threads_arch_exts[my_pid].critical_nesting == 0) {
            irq_restore(threads_arch_exts[my_pid].saved_int_state);
        }
    }
}


#endif /* DOXYGEN */
