/*
 * Copyright (C) 2019 Freie Universitaet Berlin,
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include "xtimer.h"

#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/FreeRTOS.h"

#include "semaphore_test.h"

/**
 * @brief   tests the take function for a freertos mutex semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int semaphore_test_mutex_take(void)
{
    SemaphoreHandle_t testing_semaphore = xSemaphoreCreateMutex();

    if (testing_semaphore == NULL) {
        puts("test failed: mutex semaphore not created");
        return pdFAIL;
    }
    /*
     * Freertos Documentation:
     * "pdPASS: Returned only if the call to xSemaphoreTake()
     * was successful in obtaining the semaphore."
     */
    /* first call should be successful */
    if (xSemaphoreTake(testing_semaphore, 0) == pdFAIL) {
        puts("error in Take");
        vSemaphoreDelete(testing_semaphore);
        return pdFAIL;
    }
    /* after the fist call every call should fail */
    if (xSemaphoreTake(testing_semaphore, 0) == pdPASS) {
        puts("error in Take");
        vSemaphoreDelete(testing_semaphore);
        return pdFAIL;
    }
    vSemaphoreDelete(testing_semaphore);
    return pdPASS;
}

int semaphore_test_mutex_take_not_owned(SemaphoreHandle_t testing_semaphore)
{
    if (xSemaphoreTake(testing_semaphore, 0) == pdPASS) {
        puts("error in Take: taking not owned Semaphore");
        vSemaphoreDelete(testing_semaphore);
        return pdFAIL;
    }

    if (xSemaphoreGive(testing_semaphore) == pdPASS) {
        puts("error in Give: gave not owned semaphore");
        vSemaphoreDelete(testing_semaphore);
        return pdFAIL;
    }

    if (xSemaphoreTake(testing_semaphore, 0) == pdFAIL) {
        puts("error in Take: should not be free after the thread not owning the semaphore calls xSemaphoreGive");
        vSemaphoreDelete(testing_semaphore);
        return pdFAIL;
    }
    return pdPASS;
}

int semaphore_test_mutex_thread(SemaphoreHandle_t testing_semaphore)
{

    if (xSemaphoreTake(testing_semaphore, 0) == pdFAIL) {
        puts("error in Take");
        vSemaphoreDelete(testing_semaphore);
        return pdFAIL;
    }
    /* TODO change time */
    xtimer_usleep(20);
    if (xSemaphoreGive(testing_semaphore) == pdFAIL) {
        puts("error in Give");
        vSemaphoreDelete(testing_semaphore);
        return pdFAIL;
    }
    xtimer_usleep(20);
    if (semaphore_test_mutex_take_not_owned(testing_semaphore) == pdFAIL) {
        return pdFAIL;
    }
    return pdPASS;
}

/**
 * @brief   test for the freertos mutex semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int semaphore_test_mutex(void)
{
    /*
     *
     * TODO:
     * - mulithreading
     * - create
     * - delete
     * - give
     * - take
     * 
     */
    SemaphoreHandle_t testing_semaphore = xSemaphoreCreateMutex();
    if (testing_semaphore == NULL) {
        puts("test failed: mutex semaphore not created");
        return pdFAIL;
    }
    /* TODO start thread semaphore_test_mutex_thread */
    /* TODO change time */
    xtimer_usleep(10);
    if (semaphore_test_mutex_take_not_owned(testing_semaphore) == pdFAIL) {
        return pdFAIL;
    }
    xtimer_usleep(20);

    return pdPASS;

}

/**
 * @brief   tests the take function for a freertos recursive mutex semaphore
 *
 * @return pdPASS when the test is passed, pdFAIL otherwise
 */
int semaphore_test_recursive_mutex_take(void)
{
    SemaphoreHandle_t testing_semaphore = xSemaphoreCreateRecursiveMutex();

    if (testing_semaphore == NULL) {
        puts("test failed: recursive mutex semaphore not created");
        return pdFAIL;
    }
    /*
     * Freertos Documentation:
     * "pdPASS: Returned only if the call to xSemaphoreTakeRecursive()
     * was successful in obtaining the semaphore.",
     * "once a recursive mutex has been successfully ‘taken’ by a task,
     * further calls to xSemaphoreTakeRecursive() made by the
     * same task will also be successful."
     */
    for (size_t i = 0; i < 3; i++) {
        if (xSemaphoreTakeRecursive(testing_semaphore, 0) == pdFAIL) {
            puts("error in Take");
            vSemaphoreDelete(testing_semaphore);
            return pdFAIL;
        }
    }
    return pdPASS;
}
