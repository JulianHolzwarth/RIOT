
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

static int auto_reload_test(void);
int one_shot_timer_test(void);

/* to test the task */
static SemaphoreHandle_t test_handle;

/* TODO */
static void vTimerCallback_autoreload(TimerHandle_t xTimer)
{
    DEBUG("auto reload timer callback: count of semaphore before take %d\n", uxSemaphoreGetCount(
              test_handle));
    /* has to be called twice to be a successful test */
    int32_t data = (int32_t)pvTimerGetTimerID(xTimer);
    if (data == 2) {
        xSemaphoreTake(test_handle, 0);
    }
    data++;
    vTimerSetTimerID(xTimer, (void *)data);
}

static void vTimerCallback_one_shot(TimerHandle_t xTimer)
{
    DEBUG("one shot timer callback\n");

    /* if called twice timer id will change and the test fails */
    int32_t data = (int32_t)pvTimerGetTimerID(xTimer);
    if (data == 16) {
        xSemaphoreTake(test_handle, 0);
        data++;
        vTimerSetTimerID(xTimer, (void *)data);
    }
    else {
        xSemaphoreGive(test_handle);
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

    test_handle = xSemaphoreCreateMutex();

    xSemaphoreGive(test_handle);
    DEBUG("semaphore test: %d\n", uxSemaphoreGetCount(
              test_handle));
    xSemaphoreTake(test_handle, 0);
    DEBUG("semaphore test: %d\n", uxSemaphoreGetCount(
              test_handle));
    xSemaphoreGive(test_handle);
    if (auto_reload_test() == pdFAIL) {
        test_result = pdFAIL;
    }

    xSemaphoreGive(test_handle);
    if (one_shot_timer_test() == pdFAIL) {
        test_result = pdFAIL;
    }

    vSemaphoreDelete(test_handle);
    return test_result;
}

static int auto_reload_test(void)
{
    bool test_result = pdPASS;


    TimerHandle_t testing_timer;

    testing_timer = xTimerCreate("test_timer", 5, pdTRUE, ( void * )0,
                                 vTimerCallback_autoreload);
    if (testing_timer == NULL) {
        puts("timer creation error");
        return pdFAIL;
    }
    if (xTimerStart(testing_timer, 0) == pdFAIL) {
        puts("timer did not start");
        xTimerDelete(testing_timer, 0);
        return pdFAIL;
    }
    vTaskDelay(20);

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
    return test_result;
}

int one_shot_timer_test(void)
{

    bool test_result = pdPASS;

    TimerHandle_t testing_timer;

    testing_timer = xTimerCreate("test_timer", 5, pdFALSE, ( void * )16,
                                 vTimerCallback_one_shot);
    if (testing_timer == NULL) {
        puts("timer creation error");
        return pdFAIL;
    }
    if (xTimerStart(testing_timer, 0) == pdFAIL) {
        puts("timer did not start");
        xTimerDelete(testing_timer, 0);
        return pdFAIL;
    }
    vTaskDelay(10);

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

    return test_result;
}
