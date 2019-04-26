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

#include "xtimer.h"

#include "freertos/task.h"
#include "freertos/FreeRTOS.h"

#include "include/task_test.h"

/* to test the task */
static mutex_t test_mutex;
static mutex_t finish_test_mutex;

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
    mutex_init(&finish_test_mutex);

    if (!task_test_create()) {
        test_result = pdFAIL;
    }

    if (!task_test_create_static()) {
        test_result = pdFAIL;
    }

    if (!task_test_delay()) {
        test_result = pdFAIL;
    }

    if (test_result == pdFAIL) {
        return pdFAIL;
    }
    return pdPASS;
}

int task_test_create(void)
{
    paramStruct *xParameter = &global_xParameter;

    xParameter->word1 = 'A';
    xParameter->word2 = 'B';
    mutex_unlock(&test_mutex);
    TaskHandle_t xHandle;
    xTaskCreate( task_test_thread, "testing task", THREAD_STACKSIZE_DEFAULT,
                 xParameter, 14, &xHandle );
    xtimer_sleep(1);
    if (mutex_trylock(&test_mutex) == false) {
        puts("Task creation error");
        return pdFAIL;
    }
    return pdPASS;
}

int task_test_create_static(void)
{
    paramStruct *xParameter = &global_xParameter;

    xParameter->word1 = 'A';
    xParameter->word2 = 'B';
    mutex_unlock(&test_mutex);
    StackType_t *stack = malloc(THREAD_STACKSIZE_DEFAULT);
    TaskHandle_t xHandle;
    xHandle = xTaskCreateStatic(task_test_thread, "freertos static test thread",
                                THREAD_STACKSIZE_DEFAULT, xParameter, 14, stack,
                                NULL);
    (void)xHandle;
    xtimer_sleep(1);

    if (mutex_trylock(&test_mutex) == false) {
        puts("Task creation error");
        return pdFAIL;
    }
    return pdPASS;
}

int task_test_delay(void)
{

    mutex_unlock(&test_mutex);
    mutex_unlock(&finish_test_mutex);
    TaskHandle_t xHandle;
    xTaskCreate( task_test_thread_delay, "testing task",
                 THREAD_STACKSIZE_DEFAULT, NULL, 14, &xHandle );
    if (mutex_trylock(&test_mutex) == false) {
        puts("task locked and ran too early");
        return pdFAIL;
    }
    mutex_unlock(&test_mutex);
    vTaskDelay(100);
    if (mutex_trylock(&test_mutex) == true) {
        puts("did not lock");
        return pdFAIL;
    }
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
    if (pvParameters == NULL) {
        puts("no Parameters");
        mutex_trylock(&test_mutex);
        vTaskDelete(NULL);
    }
    paramStruct pxParameters = *( paramStruct * )pvParameters;
    if (pxParameters.word1 !=  'A'  || pxParameters.word2 != 'B') {
        mutex_trylock(&test_mutex);
        puts("wrong parameter");
    }
    vTaskDelete(NULL);
}

void task_test_thread_delay(void *pvParameters)
{
    (void)pvParameters;
    mutex_trylock(&test_mutex);
    vTaskDelete(NULL);
}
