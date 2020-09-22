/*
 * Copyright (C) 2020 Freie Universität Berlin,
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       testing sema
 *
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <errno.h>
#include <stdio.h>

#include "msg.h"
#include "sema.h"
#include "thread.h"
#include "xtimer.h"

#define test_sema_value 4
#define TEST_TIME 100
#define TEST_ITERATIONS 4

static char stack[THREAD_STACKSIZE_DEFAULT];
static sema_t test_sema;
static sema_t test_sema2;
static int thread_success;

static void *second_thread(void *arg)
{
    (void) arg;
    sema_post(&test_sema);
    sema_post(&test_sema);
    printf("thread\n");
    if (sema_try_wait(&test_sema2) != 0) {
        printf("Error 8");
        thread_success = -1;
    }
    if (sema_wait_timed(&test_sema, 1000) != 0) {
        printf("Error 9");
        thread_success = -1;
    }
    if (sema_wait_timed(&test_sema2, 1000) != 0) {
        printf("Error 10");
        thread_success = -1;
    }
    if (sema_wait(&test_sema2) != -ECANCELED) {
        printf("Error 11");
        thread_success = -1;
    }
    if (thread_success != -1) {
        thread_success = 1;

    }

    return NULL;
}


int main(void)
{

    thread_success = 0;
    sema_create(&test_sema, 0);
    sema_create(&test_sema2, 1);
    if (sema_try_wait(&test_sema) != -EAGAIN) {
        printf("Error 1");
        return 1;
    }

    if (sema_wait_timed(&test_sema, 1000) != -ETIMEDOUT) {
        printf("Error 2");
        return 1;
    }

    kernel_pid_t second_pid = thread_create(stack,
                  sizeof(stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  second_thread,
                  NULL,
                  "second_thread");
    (void)second_pid;
    /* stuff */
    if (sema_wait(&test_sema) != 0){
        printf("Error 3");
        return 1;
    }
    sema_post(&test_sema2);
    printf("main\n");
    
    

    sema_destroy(&test_sema);
    sema_destroy(&test_sema2);
    sema_post(&test_sema2);
    if (sema_wait_timed(&test_sema, TEST_TIME) != -ECANCELED) {
        printf("Error 4");
        return 1;
    }
    if (sema_wait(&test_sema) != -ECANCELED) {
        printf("Error 5");
        return 1;
    }
    if (sema_try_wait(&test_sema) != -ECANCELED) {
        printf("Error 6");
        return 1;
    }
    /* UINT_MAX test */
    test_sema.value = UINT_MAX;
    if (sema_post(&test_sema) != -EOVERFLOW)
    {
        printf("Error 7");
        return 1;
    }
    
    if (thread_success == 1) {
        printf("SUCCESS\n");

    }else
    {
        printf("FAILURE\n");
    }
    

    return 0;
}
