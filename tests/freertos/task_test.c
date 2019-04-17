/*
 * Copyright (C) 2019 Freie Universitaet Berlin,
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "thread.h"
#include "mutex.h"

#include "freertos/task.h"
#include "freertos/FreeRTOS.h"

#include "include/task_test.h"

/* to test the task */
static mutex_t test_mutex;

typedef struct {
    char word1;
    char word2;
} paramStruct;

paramStruct global_xParameter;

/**
 * @brief   tests the freertos task
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int task_test_start(void)
{
    bool test_result = pdPASS;

    mutex_init(&test_mutex);

    paramStruct *xParameter = &global_xParameter;
    xParameter->word1 = 'A';
    xParameter->word2 = 'B';
    TaskHandle_t xHandle;
    xTaskCreate( task_test_thread, "testing task", THREAD_STACKSIZE_DEFAULT, (void *)xParameter, THREAD_PRIORITY_MAIN, &xHandle );
    vTaskDelay(portTICK_RATE_MS(10));
    uint32_t pid = (uint32_t )xHandle;
    thread_t *thread = (thread_t *)sched_threads[pid];
    if (thread->status != STATUS_STOPPED) {
        puts("thread did not stop after it deleted itself");
        test_result = pdFAIL;
    }

    mutex_unlock(&test_mutex);
    vTaskDelay(100);
    /* mutex no longer locked by other thread (because the thread is deleted) */
    if (mutex_trylock(&test_mutex) == false) {
        puts("task did lock after it got deleted");
        printf("status: %u \n", thread->status );
        test_result = pdFAIL;
    }
    mutex_unlock(&test_mutex);
    free(xParameter);
    vTaskDelay(100);
    if (test_result == pdFAIL) {
        return pdFAIL;
    }
    return pdPASS;
}

task_test_create()
{
    paramStruct *xParameter = &global_xParameter;
    xParameter->word1 = 'A';
    xParameter->word2 = 'B';
    TaskHandle_t xHandle;
    xTaskCreate( task_test_thread, "testing task", THREAD_STACKSIZE_DEFAULT, (void *)xParameter, THREAD_PRIORITY_MAIN, &xHandle );
    return pdPASS;
}

task_test_delete()
{
    TaskHandle_t xHandle;
    xTaskCreate( task_test_thread, "testing task", THREAD_STACKSIZE_DEFAULT, NULL, THREAD_PRIORITY_MAIN, &xHandle );

    return pdPASS;
}

task_test_delay()
{

    return pdPASS;
}

/**
 * @brief   thread for the test
 *
 * @param[in] parameter  input of the thread
 *
 * @return NULL
 */
void task_test_thread(void *pvParameters)
{
    /* check if Parameters are correct */
    paramStruct pxParameters = *( paramStruct * )pvParameters;

    if (pxParameters.word1 !=  'A'  || pxParameters.word2 != 'B') {
        mutex_trylock(&test_mutex);
        puts("wrong parameter");
        return;
    }
}

void task_delete_test(void *pvParameter)
{
    (void)pvParameter;
    vTaskDelete(NULL);
    thread_sleep();
}
