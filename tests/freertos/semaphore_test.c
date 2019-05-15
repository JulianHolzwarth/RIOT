/*
 * Copyright (C) 2019 Freie Universitaet Berlin,
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include "mutex.h"
#include "rmutex.h"
#include "thread.h"

#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/FreeRTOS.h"

#include "include/semaphore_test.h"

#ifndef SEMAPHORE_TEST_FOR_COUNTER
#define SEMAPHORE_TEST_FOR_COUNTER 100
#endif

/* to test the semaphore */
static mutex_t test_mutex;
static int8_t counting_test;

/* to test the recursive mutex semaphore */
static rmutex_t recursive_test_mutex;

/* thread stacks */
char thread_stack_char[THREAD_STACKSIZE_MAIN];
char thread_stack_char_2[THREAD_STACKSIZE_MAIN];
char thread_stack_char_3[THREAD_STACKSIZE_MAIN];
char thread_stack_char_4[THREAD_STACKSIZE_MAIN];
char thread_stack_char_5[THREAD_STACKSIZE_MAIN];

/*
 * @brief   a struct to give the threads parameters
 */
typedef struct {
    SemaphoreHandle_t sema_handle;  /* the handle of a semaphore */
    TickType_t timeout;             /* the timeout for the semaphore take call */
    bool return_val;                /* the return value from the semaphore */
}semaphore_test_parameter;



/**
 * @brief   threadfunction for the freertos mutex semaphore test and the binary semaphore
 *
 * @param[in] parameter a pointer to a struct that holds
 *                      the semaphore handle and the timeout to test the semaphroe with
 *
 * @return NULL
 */
static void *semaphore_test_thread(void *parameter)
{
    semaphore_test_parameter *parameter_struct =
        (semaphore_test_parameter *)parameter;
    SemaphoreHandle_t testing_semaphore = parameter_struct->sema_handle;
    TickType_t timeout = parameter_struct->timeout;
    int ret = pdPASS;   /* pdPass if test passed, pdFail else */
    uint8_t loop_var;

    for (size_t i = 0; i < SEMAPHORE_TEST_FOR_COUNTER; i++) {

        /* in loop until it took the semaphore once */
        loop_var = pdTRUE;
        while (loop_var) {
            if (xSemaphoreTake(testing_semaphore, timeout) == pdPASS) {
                loop_var = pdFALSE;
            }
            thread_yield();
        }
        thread_yield();

        /* if mutex could not lock the test fails because semaphore gets only locked
         * when the semaphore gets taken
         */
        if (mutex_trylock(&test_mutex) == pdFALSE) {
            ret = pdFAIL;
            puts(
                "test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        thread_yield();
        mutex_unlock(&test_mutex);
        xSemaphoreGive(testing_semaphore);
        thread_yield();
    }
    /* returning test result via struct */
    parameter_struct->return_val = ret;
    sched_task_exit();
}

/**
 * @brief   helpfunction for the freertos mutex semaphore test and the binary semaphore
 *
 * @param   testing_semaphore   the semaphore to test
 * @param   timeout             the timeout to test the semphore with
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
static int semaphore_test_helpfunction(SemaphoreHandle_t testing_semaphore,
                                       TickType_t timeout)
{
    uint8_t test_result = pdPASS;

    if (testing_semaphore == NULL) {
        puts("test failed: semaphore not created");
        return pdFAIL;
    }

    mutex_init(&test_mutex);

    /* threading */
    void *thread_stack = (void *)&thread_stack_char;
    kernel_pid_t thread_id;


    semaphore_test_parameter thread_parameter;
    thread_parameter.sema_handle = testing_semaphore;
    thread_parameter.timeout = timeout;
    thread_parameter.return_val = pdPASS;
    thread_id = thread_create(thread_stack, THREAD_STACKSIZE_MAIN,
                              THREAD_PRIORITY_MAIN, THREAD_CREATE_WOUT_YIELD,
                              semaphore_test_thread, (void *)&thread_parameter,
                              "semaphore_helpfunction");
    if (!pid_is_valid(thread_id)) {
        puts("Error in thread creation: pid not valid");
        return pdFAIL;
    }

    /* semaphore test */
    uint8_t loop_var;
    for (size_t i = 0; i < SEMAPHORE_TEST_FOR_COUNTER; i++) {

        /* in loop until it took the semaphore once */
        loop_var = pdTRUE;
        while (loop_var) {
            if (xSemaphoreTake(testing_semaphore, timeout) == pdPASS) {
                loop_var = pdFALSE;
            }
            thread_yield();
        }
        thread_yield();

        /* if mutex could not lock the test fails because semaphore gets only locked
         * when the semaphore gets taken
         */
        if (mutex_trylock(&test_mutex) == pdFALSE) {
            test_result = pdFAIL;
            puts(
                "test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        thread_yield();
        mutex_unlock(&test_mutex);
        xSemaphoreGive(testing_semaphore);
        thread_yield();
    }

    /* waiting for created thread to finish */
    while (thread_getstatus(thread_id) > (int)STATUS_STOPPED) {
        thread_yield();
    }

    /* evaluating test results */
    if (test_result == pdFAIL || thread_parameter.return_val == pdFAIL) {
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
    bool test_result = pdPASS;
    SemaphoreHandle_t testing_semaphore = xSemaphoreCreateMutex();

    /* testing mutex semaphore with no timeout */
    if (semaphore_test_helpfunction(testing_semaphore, 0) == pdFAIL) {
        test_result = pdFAIL;
    }
    /* testing mutex semaphore with max timeout */
    if (semaphore_test_helpfunction(testing_semaphore,
                                    portMAX_DELAY) == pdFAIL) {
        test_result = pdFAIL;
    }
    vSemaphoreDelete(testing_semaphore);

    return test_result;
}

/**
 * @brief   tests the freertos binary semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int semaphore_test_binary(void)
{
    bool test_result = pdPASS;
    SemaphoreHandle_t testing_semaphore = xSemaphoreCreateBinary();
    /* binary semaphore starts already taken */
    xSemaphoreGive(testing_semaphore);

    /* testing mutex semaphore with no timeout */
    if (semaphore_test_helpfunction(testing_semaphore, 0) == pdFAIL) {
        test_result = pdFAIL;
    }
    /* testing mutex semaphore with max timeout */
    if (semaphore_test_helpfunction(testing_semaphore,
                                    portMAX_DELAY) == pdFAIL) {
        test_result = pdFAIL;
    }
    vSemaphoreDelete(testing_semaphore);

    return test_result;
}

/**
 * @brief   threadfunction for the freertos recursive recursive mutex semaphore test
 *
 * @param[in] parameter a pointer to a struct that holds
 *                      the semaphore handle and the timeout to test the semaphroe with
 *
 * @return NULL
 */
static void *semaphore_test_recursive_mutex_thread(void *parameter)
{

    semaphore_test_parameter *parameter_struct =
        (semaphore_test_parameter *)parameter;
    SemaphoreHandle_t testing_semaphore = parameter_struct->sema_handle;
    TickType_t timeout = parameter_struct->timeout;
    int ret = pdPASS;


    uint8_t loop_var;

    for (size_t i = 0; i < SEMAPHORE_TEST_FOR_COUNTER; i++) {

        /* in loop until it took the semaphore once */
        loop_var = pdTRUE;
        while (loop_var) {
            if (xSemaphoreTake(testing_semaphore, timeout) == pdPASS) {
                loop_var = pdFALSE;
            }
            thread_yield();
        }
        thread_yield();
        /* if mutex could not lock the test fails because semaphore gets only locked
         * when the semaphore gets taken
         */
        if (rmutex_trylock(&recursive_test_mutex) == pdFALSE) {
            ret = pdFAIL;
            puts(
                "test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        /* because the semaphore is owned by this thread, the thread should be able to take the semaphore again */
        thread_yield();
        if (xSemaphoreTake(testing_semaphore, timeout) == pdFAIL) {
            ret = pdFAIL;
            puts(
                "test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        thread_yield();
        /* if mutex could not lock the test fails because semaphore gets only locked
         * when the semaphore gets taken
         */
        if (rmutex_trylock(&recursive_test_mutex) == pdFALSE) {
            ret = pdFAIL;
            puts(
                "test failed: xSemaphoreTake() succeded for a semaphore without free places");
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
        parameter_struct->return_val = pdFAIL;
    }
    sched_task_exit();
}

/**
 * @brief   helpfunction to test the freertos recursive mutex semaphore
 *
 * @param   timeout     the timeout to test the semaphore with
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
static int semaphore_test_recursive_mutex_helpfunc(TickType_t timeout)
{
    uint8_t test_result = pdPASS;

    SemaphoreHandle_t testing_semaphore =  xSemaphoreCreateRecursiveMutex();

    if (testing_semaphore == NULL) {
        puts("test failed: recursive mutex semaphore not created");
        return pdFAIL;
    }

    rmutex_init(&recursive_test_mutex);

    /* parameters for thread */
    semaphore_test_parameter thread_parameter;
    thread_parameter.sema_handle = testing_semaphore;
    thread_parameter.timeout = timeout;
    thread_parameter.return_val = pdPASS;

    /* threading */
    void *thread_stack = (void *)thread_stack_char;
    kernel_pid_t thread_id;
    thread_id = thread_create(thread_stack, THREAD_STACKSIZE_MAIN,
                              THREAD_PRIORITY_MAIN, THREAD_CREATE_WOUT_YIELD,
                              &semaphore_test_recursive_mutex_thread,
                              (void *)&thread_parameter,
                              "semaphore_helpfunction");
    if (!pid_is_valid(thread_id)) {
        vSemaphoreDelete(testing_semaphore);
        puts("Error in thread creation: pid not valid");
        return pdFAIL;
    }

    /* semaphore test */
    uint8_t loop_var;
    for (size_t i = 0; i < SEMAPHORE_TEST_FOR_COUNTER; i++) {

        /* in loop until it took the semaphore once */
        loop_var = pdTRUE;
        while (loop_var) {
            if (xSemaphoreTake(testing_semaphore, timeout) == pdPASS) {
                loop_var = pdFALSE;
            }
            thread_yield();
        }
        thread_yield();
        /* if mutex could not lock the test fails because semaphore gets only locked
         * when the semaphore gets taken
         */
        if (rmutex_trylock(&recursive_test_mutex) == pdFALSE) {
            test_result = pdFAIL;
            puts(
                "test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        /* because the semaphore is owned by this thread, the thread should be able to take the semaphore again */
        thread_yield();
        if (xSemaphoreTake(testing_semaphore, timeout) == pdFAIL) {
            test_result = pdFAIL;
            puts(
                "test failed: xSemaphoreTake() succeded for a semaphore without free places");
            break;
        }
        thread_yield();
        /* if mutex could not lock the test fails because semaphore gets only locked
         * when the semaphore gets taken
         */
        if (rmutex_trylock(&recursive_test_mutex) == pdFALSE) {
            test_result = pdFAIL;
            puts(
                "test failed: xSemaphoreTake() succeded for a semaphore without free places");
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
    while (thread_getstatus(thread_id) > (int)STATUS_STOPPED) {
        thread_yield();
    }

    /* evaluating test results */
    vSemaphoreDelete(testing_semaphore);
    if (test_result == pdFAIL || thread_parameter.return_val == pdFAIL) {
        return pdFAIL;
    }
    return pdPASS;
}

/**
 * @brief   tests the freertos recursive mutex semaphore
 *
 * @return pdPASS when the test is passed, pdFAIL otherwise
 */
int semaphore_test_recursive_mutex(void)
{
    bool test_result = pdPASS;

    /* testing mutex semaphore with no timeout */
    if (semaphore_test_recursive_mutex_helpfunc(0) == pdFAIL) {
        test_result = pdFAIL;
    }
    /* testing mutex semaphore with max timeout */
    if (semaphore_test_recursive_mutex_helpfunc(portMAX_DELAY) == pdFAIL) {
        test_result = pdFAIL;
    }
    return test_result;
}

/**
 * @brief   threadfunction for the freertos counting mutex semaphore test
 *
 * @param[in] parameter a pointer to a struct that holds
 *                      the semaphore handle and the timeout to test the semaphroe with
 *
 * @return NULL
 */

static void *semaphore_test_counting_thread(void *parameter)
{

    semaphore_test_parameter *parameter_struct =
        (semaphore_test_parameter *)parameter;
    SemaphoreHandle_t testing_semaphore = parameter_struct->sema_handle;
    TickType_t timeout = parameter_struct->timeout;
    int ret = pdPASS;

    uint8_t loop_var;

    for (size_t i = 0; i < SEMAPHORE_TEST_FOR_COUNTER; i++) {
        loop_var = pdTRUE;
        while (loop_var) {
            if (xSemaphoreTake(testing_semaphore, timeout) == pdPASS) {
                loop_var = pdFALSE;
            }
            thread_yield();
        }
        thread_yield();
        mutex_lock(&test_mutex);
        if (counting_test <= 0) {
            ret = pdFAIL;
            puts(
                "test failed: xSemaphoreTake() succeded for a semaphore without free places");
            mutex_unlock(&test_mutex);
            break;
        }
        counting_test--;
        mutex_unlock(&test_mutex);
        thread_yield();

        mutex_lock(&test_mutex);
        if (counting_test >= 5) {
            ret = pdFAIL;
            puts("test failed: too many places");
            mutex_unlock(&test_mutex);
            break;
        }
        thread_yield();
        counting_test++;
        thread_yield();
        mutex_unlock(&test_mutex);
        xSemaphoreGive(testing_semaphore);
        thread_yield();
    }

    if (ret == pdFAIL && parameter_struct->return_val == pdPASS) {
        parameter_struct->return_val = pdFAIL;
    }
    sched_task_exit();
}

/**
 * @brief   helpfunction to test the freertos counting semaphore
 *
 * @param   timeout     the timeout to test the semaphore with
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
static int semaphore_test_counting_helpfunc(TickType_t timeout)
{
    uint8_t test_result = pdPASS;
    SemaphoreHandle_t testing_semaphore = xSemaphoreCreateCounting(5, 2);

    counting_test = 2;
    if (testing_semaphore == NULL) {
        puts("test failed: counting semaphore not created");
        return pdFAIL;
    }

    mutex_init(&test_mutex);

    void *thread1_stack = (void *)&thread_stack_char;
    void *thread2_stack = (void *)&thread_stack_char_2;
    void *thread3_stack = (void *)&thread_stack_char_3;
    void *thread4_stack = (void *)&thread_stack_char_4;
    void *thread5_stack = (void *)&thread_stack_char_5;

    semaphore_test_parameter thread_parameter;
    thread_parameter.sema_handle = testing_semaphore;
    thread_parameter.timeout = timeout;
    thread_parameter.return_val = pdPASS;

    kernel_pid_t thread_id_1;
    kernel_pid_t thread_id_2;
    kernel_pid_t thread_id_3;
    kernel_pid_t thread_id_4;
    kernel_pid_t thread_id_5;

    thread_id_1 = thread_create(thread1_stack, THREAD_STACKSIZE_MAIN,
                                THREAD_PRIORITY_MAIN, THREAD_CREATE_WOUT_YIELD,
                                &semaphore_test_counting_thread,
                                (void *)&thread_parameter,
                                "c_semaphore_helpfunction_1");
    thread_id_2 = thread_create(thread2_stack, THREAD_STACKSIZE_MAIN,
                                THREAD_PRIORITY_MAIN, THREAD_CREATE_WOUT_YIELD,
                                &semaphore_test_counting_thread,
                                (void *)&thread_parameter,
                                "c_semaphore_helpfunction_2");
    thread_id_3 = thread_create(thread3_stack, THREAD_STACKSIZE_MAIN,
                                THREAD_PRIORITY_MAIN, THREAD_CREATE_WOUT_YIELD,
                                &semaphore_test_counting_thread,
                                (void *)&thread_parameter,
                                "c_semaphore_helpfunction_3");
    thread_id_4 = thread_create(thread4_stack, THREAD_STACKSIZE_MAIN,
                                THREAD_PRIORITY_MAIN, THREAD_CREATE_WOUT_YIELD,
                                &semaphore_test_counting_thread,
                                (void *)&thread_parameter,
                                "c_semaphore_helpfunction_4");
    thread_id_5 = thread_create(thread5_stack, THREAD_STACKSIZE_MAIN,
                                THREAD_PRIORITY_MAIN, THREAD_CREATE_WOUT_YIELD,
                                &semaphore_test_counting_thread,
                                (void *)&thread_parameter,
                                "c_semaphore_helpfunction_5");

    if ((!pid_is_valid(thread_id_1)) && (!pid_is_valid(thread_id_2)) &&
        (!pid_is_valid(thread_id_3)) &&
        (!pid_is_valid(thread_id_4)) && (!pid_is_valid(thread_id_5))) {
        vSemaphoreDelete(testing_semaphore);
        puts("Error in thread creation: pid not valid");
        return pdFAIL;
    }

    /* semaphore test */
    uint8_t loop_var;
    for (size_t i = 0; i < SEMAPHORE_TEST_FOR_COUNTER; i++) {
        loop_var = pdTRUE;
        while (loop_var) {
            if (xSemaphoreTake(testing_semaphore, timeout) == pdPASS) {
                loop_var = pdFALSE;
            }
            thread_yield();
        }
        thread_yield();
        mutex_lock(&test_mutex);
        if (counting_test <= 0) {
            test_result = pdFAIL;
            puts(
                "test failed: xSemaphoreTake() succeded for a semaphore without free places");
            mutex_unlock(&test_mutex);
            break;
        }
        thread_yield();
        counting_test--;
        thread_yield();
        mutex_unlock(&test_mutex);
        thread_yield();

        mutex_lock(&test_mutex);
        if (counting_test >= 5) {
            test_result = pdFAIL;
            puts("test failed: too many places");
            mutex_unlock(&test_mutex);
            break;
        }
        thread_yield();
        counting_test++;
        thread_yield();
        mutex_unlock(&test_mutex);

        xSemaphoreGive(testing_semaphore);
        thread_yield();
    }

    /* waiting for created thread to finish */
    while (thread_getstatus(thread_id_1) > (int)STATUS_STOPPED ||
           thread_getstatus(thread_id_2) > (int)STATUS_STOPPED ||
           thread_getstatus(thread_id_3) > (int)STATUS_STOPPED ||
           thread_getstatus(thread_id_4) > (int)STATUS_STOPPED ||
           thread_getstatus(thread_id_5) > (int)STATUS_STOPPED) {
        thread_yield();
    }

    vSemaphoreDelete(testing_semaphore);
    /* evaluating test results */
    if (test_result == pdFAIL || thread_parameter.return_val == pdFAIL) {
        return pdFAIL;
    }
    return pdPASS;
}

/**
 * @brief   tests the freertos counting semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int semaphore_test_counting(void)
{
    bool test_result = pdPASS;

    if (semaphore_test_counting_helpfunc(0) == pdFAIL) {
        test_result = pdFAIL;
    }
    if (semaphore_test_counting_helpfunc(portMAX_DELAY) == pdFAIL) {
        test_result = pdFAIL;
    }
    return test_result;
}
