
/*
 * Copyright (C) 2019 Freie Universitaet Berlin,
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#define ENABLE_DEBUG (0)
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "thread.h"
#include "mutex.h"

#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"


#include "include/timer_test.h"

/* to test the task */
static SemaphoreHandle_t test_handle;

/* TODO */
static void vTimerCallback(TimerHandle_t xTimer)
{
    DEBUG("timer callback: count of semaphore before take %d\n", uxSemaphoreGetCount(
              test_handle));
    xSemaphoreTake(test_handle, 0);
    int32_t data = (int32_t)pvTimerGetTimerID(xTimer);
    if (data == 1) {
        puts("yeah\n");
    }
    else {
        data++;
        vTimerSetTimerID(xTimer, (void *)data);
    }
}


/**
 * @brief   tests the freertos timer
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int timer_test_start(void)
{
    bool test_result = pdPASS;

    test_handle =  xQueueCreateCountingSemaphore(3, 3);

    TimerHandle_t testing_timer;
    testing_timer = xTimerCreate("test_timer", 10, pdTRUE, ( void * )0,
                                 vTimerCallback);
    if (testing_timer == NULL) {
        puts("timer creation error");
        vSemaphoreDelete(test_handle);
        return pdFAIL;
    }
    if (xTimerStart(testing_timer, 0) == pdFAIL) {
        puts("timer did not start");
        xTimerDelete(testing_timer, 0);
        vSemaphoreDelete(test_handle);
        return pdFAIL;
    }
    vTaskDelay(50);

    if (uxSemaphoreGetCount(test_handle) != 0) {
        printf("count %d\n", uxSemaphoreGetCount(test_handle));
        puts("timer callback function did not run");
        test_result = pdFAIL;
    }
    if (xTimerStop(testing_timer, 0) == pdFAIL) {
        puts("timer did not stop");
        test_result = pdFAIL;
    }
    if (xTimerDelete(testing_timer, 0) == pdFAIL) {
        puts("timer delete error");
        test_result = pdFAIL;
    }
    vSemaphoreDelete(test_handle);
    if (test_result == pdFAIL) {
        return pdFAIL;
    }
    return pdPASS;
}
