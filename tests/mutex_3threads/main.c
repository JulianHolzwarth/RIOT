/*
 * Copyright (C) 2020 Freie Universität Berlin,
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   simple test application for atomic mutex unlocking and sleeping
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include "thread.h"
#include "mutex.h"

static mutex_t mutex = MUTEX_INIT;
static kernel_pid_t main_pid;
static char stack[THREAD_STACKSIZE_DEFAULT];
static char stack2[THREAD_STACKSIZE_DEFAULT];

static void *second_thread(void *arg)
{
    (void) arg;
    mutex_lock(&mutex);
    puts("2nd");
    thread_wakeup(main_pid);
    mutex_unlock_and_sleep(&mutex);

    return NULL;
}

static void *third_thread(void *arg)
{
    (void) arg;
    mutex_lock(&mutex);
    puts("3rd");
    mutex_unlock_and_sleep(&mutex);
    
    return NULL;
}

int main(void)
{

    main_pid = thread_getpid();

    thread_create(stack,
                  sizeof(stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  second_thread,
                  NULL,
                  "second_thread");
    thread_create(stack2,
                  sizeof(stack2),
                  THREAD_PRIORITY_MAIN - 2,
                  THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  third_thread,
                  NULL,
                  "third_thread");
    mutex_lock(&mutex);
    thread_yield_higher();
    mutex_unlock_and_sleep(&mutex);
    puts("SUCCESS");
    return 0;

}
