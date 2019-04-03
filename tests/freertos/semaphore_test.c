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
#include "log.h"
#include "mutex.h"
#include "rmutex.h"
#include "thread.h"

#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/FreeRTOS.h"

#include "semaphore_test.h"

#ifndef SEMAPHORE_TEST_FOR_COUNTER
#define SEMAPHORE_TEST_FOR_COUNTER 1000
#endif

/* to test the semaphore */
static mutex_t test_mutex;
/* to test the recursive mutex semaphore */
static rmutex_t recursive_test_mutex;
/* if locked thread is done */
static mutex_t thread_done_mutex;
/* if locked at least one thread failed the test */
static mutex_t thread_return_value_mutex;


/* give does not work as in freertos: give allways works in riot (gives not owned semaphores) */

/**
 * @brief   threadfunction for the freertos mutex semaphore test and the binary semaphore
 *
 * @param[in] parameter  the SemaphoreHandle of the semaphore to be tested
 * 
 * @return NULL
 */

static void * semaphore_test_thread(void *parameter)
{
    SemaphoreHandle_t testing_semaphore = (SemaphoreHandle_t) parameter;
    int ret = pdPASS;

    uint8_t loop_var;
    for(size_t i = 0; i < SEMAPHORE_TEST_FOR_COUNTER; i++)
    {
        loop_var = pdTRUE;
        while(loop_var) {
            if (xSemaphoreTake(testing_semaphore, 0) == pdPASS) {
                loop_var = pdFALSE;
            }
            thread_yield();
        }
        thread_yield();
        if (mutex_trylock(&test_mutex) == pdFALSE){
            ret = pdFAIL;
            puts("test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        mutex_unlock(&test_mutex);
        xSemaphoreGive(testing_semaphore);
        thread_yield();
    }
    if (ret == pdFAIL) {
        mutex_trylock(&thread_return_value_mutex);
    }
    mutex_trylock(&thread_done_mutex);
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
    mutex_init(&thread_done_mutex);
    mutex_init(&thread_return_value_mutex);

    /* threading */
    void *thread_stack = malloc(THREAD_STACKSIZE_MAIN);
    kernel_pid_t thread_id;

    thread_id = thread_create(thread_stack, THREAD_STACKSIZE_MAIN,
                              THREAD_PRIORITY_MAIN, THREAD_CREATE_WOUT_YIELD, 
                              semaphore_test_thread, (void *) testing_semaphore, "semaphore_helpfunction");
    if (!pid_is_valid(thread_id)) {
        free(thread_stack);
        vSemaphoreDelete(testing_semaphore);
        puts("Error in thread creation: pid not valid");
        return pdFAIL;
    }

    /* semaphore test */
    uint8_t loop_var;
    for(size_t i = 0; i < SEMAPHORE_TEST_FOR_COUNTER; i++)
    {
        loop_var = pdTRUE;
        while(loop_var) {
            if (xSemaphoreTake(testing_semaphore, 0) == pdPASS) {
                loop_var = pdFALSE;
            }
            thread_yield();
        }
        thread_yield();
        if (mutex_trylock(&test_mutex) == pdFALSE){
            vSemaphoreDelete(testing_semaphore);
            test_result = pdFAIL;
            puts("test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        mutex_unlock(&test_mutex);
        xSemaphoreGive(testing_semaphore);
        thread_yield();
    }

   /* waiting for created thread to finish */
   bool thread_done_val = pdFALSE;
   while(!thread_done_val){
       thread_yield();
       if ((&thread_done_mutex)->queue.next != NULL) {
           thread_done_val = pdTRUE;
       }
   }

   /* evaluating test results and freeing memory */
   free(thread_stack);
   if (test_result == pdFAIL || mutex_trylock(&thread_return_value_mutex) == pdFALSE ) {
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
 *          NOT USED ANYMORE
 *          TODO DELETE
 *
 * @return pdPASS when the test is passed, pdFAIL otherwise
 */
int semaphore_test_recursive_mutex_take(void)
{
    /* TODO */
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

/**
 * @brief   threadfunction for the freertos recursive recursive mutex semaphore test
 *
 * @param[in] parameter  the SemaphoreHandle of the semaphore to be tested
 * 
 * @return NULL
 */

static void * semaphore_test_recursive_mutex_thread(void *parameter)
{
    SemaphoreHandle_t testing_semaphore = (SemaphoreHandle_t) parameter;
    int ret = pdPASS;

    uint8_t loop_var;
    for(size_t i = 0; i < SEMAPHORE_TEST_FOR_COUNTER; i++)
    {
        loop_var = pdTRUE;
        while(loop_var) {
            if (xSemaphoreTake(testing_semaphore, 0) == pdPASS) {
                loop_var = pdFALSE;
            }
            thread_yield();
        }
        thread_yield();
        if (rmutex_trylock(&recursive_test_mutex) == pdFALSE){
            ret = pdFAIL;
            puts("test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        /* because the semaphore is owned by this thread, the thread should be able to take the semaphore again */
        thread_yield();
        if (xSemaphoreTake(testing_semaphore, 0) == pdFAIL) {
            ret = pdFAIL;
            puts("test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
            }
        thread_yield();
        if (rmutex_trylock(&recursive_test_mutex) == pdFALSE){
            ret = pdFAIL;
            puts("test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        thread_yield();
        rmutex_unlock(&recursive_test_mutex);
        xSemaphoreGive(testing_semaphore);

        thread_yield();
        rmutex_unlock(&recursive_test_mutex);
        xSemaphoreGive(testing_semaphore);
        thread_yield();
    }
    if (ret == pdFAIL) {
        mutex_trylock(&thread_return_value_mutex);
    }
    mutex_trylock(&thread_done_mutex);
    return NULL;
}

/**
 * @brief   tests the freertos recursive mutex semaphore
 *
 * @return pdPASS when the test is passed, pdFAIL otherwise
 */
int semaphore_test_recursive_mutex(void)
{
    SemaphoreHandle_t testing_semaphore =  xSemaphoreCreateRecursiveMutex();
    uint8_t test_result = pdPASS;
    if (testing_semaphore == NULL) {
        puts("test failed: mutex semaphore not created");
        return pdFAIL;
    }
    puts("mutex init");
    rmutex_init(&recursive_test_mutex);
    mutex_init(&thread_done_mutex);
    mutex_init(&thread_return_value_mutex);

    /* threading */
    void *thread_stack = malloc(THREAD_STACKSIZE_MAIN);
    kernel_pid_t thread_id;
    thread_id = thread_create(thread_stack, THREAD_STACKSIZE_MAIN,
                              THREAD_PRIORITY_MAIN, THREAD_CREATE_WOUT_YIELD, 
                              &semaphore_test_recursive_mutex_thread, (void *) testing_semaphore, "semaphore_helpfunction");
    if (!pid_is_valid(thread_id)) {
        free(thread_stack);
        vSemaphoreDelete(testing_semaphore);
        puts("Error in thread creation: pid not valid");
        return pdFAIL;
    }
    uint8_t loop_var;
    for(size_t i = 0; i < SEMAPHORE_TEST_FOR_COUNTER; i++)
    {
        loop_var = pdTRUE;
        while(loop_var) {
            if (xSemaphoreTake(testing_semaphore, 0) == pdPASS) {
                loop_var = pdFALSE;
            }
            thread_yield();
        }
        thread_yield();
        if (rmutex_trylock(&recursive_test_mutex) == pdFALSE){
            test_result = pdFAIL;
            puts("test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        /* because the semaphore is owned by this thread, the thread should be able to take the semaphore again */
        thread_yield();
        if (xSemaphoreTake(testing_semaphore, 0) == pdFAIL) {
            test_result = pdFAIL;
            puts("test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
            }
        thread_yield();
        if (rmutex_trylock(&recursive_test_mutex) == pdFALSE){
            test_result = pdFAIL;
            puts("test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        thread_yield();
        rmutex_unlock(&recursive_test_mutex);
        xSemaphoreGive(testing_semaphore);
        
        thread_yield();
        rmutex_unlock(&recursive_test_mutex);
        xSemaphoreGive(testing_semaphore);
        thread_yield();
    }

   /* waiting for created thread to finish */
   bool thread_done_val = pdFALSE;
   while(!thread_done_val){
       thread_yield();
       if ((&thread_done_mutex)->queue.next != NULL) {
           thread_done_val = pdTRUE;
       }
   }

   /* evaluating test results and freeing memory */
   free(thread_stack);
   if (test_result == pdFAIL || mutex_trylock(&thread_return_value_mutex) == pdFALSE ) {
       vSemaphoreDelete(testing_semaphore);
       return pdFAIL;
   }
    vSemaphoreDelete(testing_semaphore);
    return pdPASS;
}

/**
 * @brief   tests the freertos counting semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int semaphore_test_counting(void)
{
    /* TODO */
    return pdFAIL;
}
