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
#include "pthread.h"
#include "xtimer.h"
#include "mutex.h"

#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/FreeRTOS.h"

#include "semaphore_test.h"

static mutex_t test_mutex;

/* give does not work as in freertos: give allways works in riot (gives not owned semaphores) */

/**
 * @brief   threadfunction for the freertos mutex semaphore test and the binary semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */

static void * semaphore_test_mutex_thread(void *parameter)
{
    SemaphoreHandle_t testing_semaphore = (SemaphoreHandle_t) parameter;
    int ret = pdPASS;

    uint8_t loop_var;
    for(size_t i = 0; i < 1000; i++)
    {
        loop_var = pdTRUE;
        while(loop_var) {
            if (xSemaphoreTake(testing_semaphore, 0) == pdPASS) {
                loop_var = pdFALSE;
            }
        }
        xtimer_usleep(10);
        if (mutex_trylock(&test_mutex) == pdFALSE){
            vSemaphoreDelete(testing_semaphore);
            ret = pdFAIL;
            puts("test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        mutex_unlock(&test_mutex);
        xSemaphoreGive(testing_semaphore);
    }
    pthread_exit((void*)ret);
    return NULL;
}

/**
 * @brief   helpfunction for the freertos mutex semaphore test and the binary semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
static int semaphore_test_helpfunction(SemaphoreHandle_t testing_semaphore) {
    uint8_t test_result = pdPASS;
    if (testing_semaphore == NULL) {
        puts("test failed: mutex semaphore not created");
        return pdFAIL;
    }

    mutex_init(&test_mutex);
    mutex_unlock(&test_mutex);
    pthread_t sem_thread;
    pthread_attr_t sem_attr;
    pthread_attr_init(&sem_attr);
    pthread_create(&sem_thread, &sem_attr, semaphore_test_mutex_thread, (void *) testing_semaphore);
    
    uint8_t loop_var;
    for(size_t i = 0; i < 1000; i++)
    {
        loop_var = pdTRUE;
        while(loop_var) {
            if (xSemaphoreTake(testing_semaphore, 0) == pdPASS) {
                loop_var = pdFALSE;
            }
        }
        xtimer_usleep(10);
        if (mutex_trylock(&test_mutex) == pdFALSE){
            vSemaphoreDelete(testing_semaphore);
            test_result = pdFAIL;
            puts("test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        mutex_unlock(&test_mutex);
        xSemaphoreGive(testing_semaphore);
    }

    int ret;
    if (pthread_join(sem_thread, (void **) &ret) != 0) {
        puts("pthread error");
        vSemaphoreDelete(testing_semaphore);
        return pdFAIL;
    }

    if(ret == pdFAIL || test_result == pdFAIL){
        vSemaphoreDelete(testing_semaphore);
        return pdFAIL;
    }
    vSemaphoreDelete(testing_semaphore);
    return pdPASS;

}

/**
 * @brief   test for the freertos mutex semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int semaphore_test_mutex(void)
{
    SemaphoreHandle_t testing_semaphore = xSemaphoreCreateMutex();
    return semaphore_test_helpfunction(testing_semaphore);
}

/**
 * @brief   tests the freertos binary semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int semaphore_test_binary(void)
{
    SemaphoreHandle_t testing_semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(testing_semaphore);
    return semaphore_test_helpfunction(testing_semaphore);
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
