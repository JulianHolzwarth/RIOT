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
 * @brief       Implementation of freertos tasks
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 * This file is needed because esp32 has specfic functions.
 * Implemented is the creation of a freetos task, the delay and
 * delete. The scheduler is still riots. Please use
 * xTaskCreateStatic() instead of xTaskCreate(). To use xTaskCreate
 * configSUPPORT_DYNAMIC_ALLOCATION (in file FreeRTOSConfig.h)
 * must be true.
 *
 * @author      Gunar Schorcht
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#ifndef FREERTOS_TASK_COMMON_H
#define FREERTOS_TASK_COMMON_H

#ifndef DOXYGEN

#include "thread.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define xTaskHandle     TaskHandle_t
#define tskNO_AFFINITY  INT_MAX
#define errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY pdFAIL

typedef void (*TaskFunction_t)(void *);

typedef void *TaskHandle_t;
typedef TaskHandle_t StaticTask_t;
typedef char *StackType_t;

#if configSUPPORT_DYNAMIC_ALLOCATION
/**
 * @brief       creates a new instance of a task. as a riot thread
 *              with dynamic memory allocation
 *              the task works like riot threads
 *              only useable if configSUPPORT_DYNAMIC_ALLOCATION
 *              in freertos/FreeRTOSConfig.h
 *
 *              please try to use xTaskCreateStatic()
 *
 * @param   pvTaskCode      the starting function
 * @param   pcName          the name of the task
 * @param   usStackDepth    the stack lenght in bytes
 * @param   pvParameters    the parameters to give the task
 * @param   uxPriority      the freertos priority (inverted riot priority)
 * @param   pvCreatedTask   the task handle
 *
 * @return  pdTrue                                  if  successful
 * @return  errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY   if not enough memory for the stack creation
 */
BaseType_t xTaskCreate(TaskFunction_t pvTaskCode,
                       const char *const pcName,
                       const uint32_t usStackDepth,
                       void *const pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t *const pvCreatedTask);
#endif

/**
 * @brief       creates a new instance of a task as a riot thread
 *              the ram for the stack is provided by
 *              the caller of this funcion
 *              the task works like riot threads
 *
 *
 * @param   pvTaskCode      the starting function
 * @param   pcName          the name of the task
 * @param   ulStackDepth    the lenght of memory the puxStackBuffer pointing to in bytes
 * @param   pvParameters    the parameters to give the task
 * @param   uxPriority      the freertos priority (inverted riot priority)
 * @param   puxStackBuffer  the pointer to the stack
 *                          (space for TCB will get used from it like in riot threads)
 * @param   pxTaskBuffer    not used (normally pointer to memory for TCB)
 *
 * @return  the task handle if  successful
 * @return  NULL            else
 */
TaskHandle_t xTaskCreateStatic(TaskFunction_t pvTaskCode,
                               const char *const pcName,
                               uint32_t ulStackDepth,
                               void *pvParameters,
                               UBaseType_t uxPriority,
                               StackType_t *const puxStackBuffer,
                               StaticTask_t *const pxTaskBuffer);

/**
 * @brief       deletes a freertos task
 *              if xTaskToDelete == NULL the task
 *              will kill itself
 *              in riot you cannot kill another thread
 *              so this function can only be used to kill
 *              the calling task
 *
 *
 * @param   xTaskToDelete      the task to kill
 */
void vTaskDelete(TaskHandle_t xTaskToDelete);

/**
 * @brief       delays the calling task
 *              the task will sleep
 *
 * @param   xTicksToDelay   the number of ticks to delay the task
 */
void vTaskDelay(const TickType_t xTicksToDelay);

/**
 * @brief       gives the freertos task handle of
 *              the currently running task
 *
 * @return  the task handle
 */
TaskHandle_t xTaskGetCurrentTaskHandle(void);

/**
 * @brief       to enter a critical section
 *              interrupt gets turned off
 *
 * @param       mux     port mutex
 */
void vTaskEnterCritical(portMUX_TYPE *mux);

/**
 * @brief       to exit a critical section
 *              interrupt gets turnes on
 *
 * @param       mux     port mutex
 */
void vTaskExitCritical(portMUX_TYPE *mux);

/**
 * @brief       converts milliseconds into port ticks
 *
 * @param       ms      number of millisecond to convert
 *
 * @return      number of ticks as TickType_t
 */
TickType_t portTICK_RATE_MS(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif  /* DOXYGEN */
#endif  /* FREERTOS_TASK_COMMON_H */
