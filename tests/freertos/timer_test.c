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

#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"

#include "task_test.h"

/* to test the task */
static mutex_t test_mutex;
 

static void vTimerCallback(TimerHandle_t xTimer)
{
    (void) xTimer;
    mutex_trylock(&test_mutex);
    //puts("time !");

}


/**
 * @brief   tests the freertos timer
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int timer_test_start(void)
{
    bool test_result = pdPASS;
    mutex_init(&test_mutex);
    TimerHandle_t testing_timer;
    testing_timer = xTimerCreate("test_timer", 10, pdTRUE, ( void * ) 0, vTimerCallback);
    if (testing_timer == NULL) {
        puts("timer creation error");
        return pdFAIL;
    }
    if (xTimerStart(testing_timer, 0) == pdFAIL) {
        puts("timer did not start");
        test_result = pdFAIL;
        xTimerDelete(testing_timer, 0);
        return test_result;
    }
    vTaskDelay(10);
    if (mutex_trylock(&test_mutex) == pdTRUE) {
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
    
    if (test_result == pdFAIL) {
        return pdFAIL;
    }
    return pdPASS;
}
