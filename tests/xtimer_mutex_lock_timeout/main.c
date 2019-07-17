/*
 * Copyright (C) 2019 Freie Universität Berlin,
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
 * @brief       testing xtimer_mutex_lock_timeout function
 *
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <stdio.h>
#include "shell.h"
#include "xtimer.h"
#include "thread.h"
#include "msg.h"
#include "irq.h"

/* timeout at one millisecond (1000 us) to make sure it does not spin. */
#define LONG_MUTEX_TIMEOUT 1000

/* timeout smaller than XTIMER_BACKOFF to make sure it spins. */
#define SHORT_MUTEX_TIMEOUT ((1 << XTIMER_SHIFT) + 1)

/* main Thread PID */
kernel_pid_t main_thread_pid;

/**
 * Foward declarations
 */
static int cmd_test_xtimer_mutex_lock_timeout_long_unlocked(int argc,
                                                            char **argv);
static int cmd_test_xtimer_mutex_lock_timeout_long_locked(int argc,
                                                          char **argv);
static int cmd_test_xtimer_mutex_lock_timeout_low_prio_thread(int argc,
                                                              char **argv);
static int cmd_test_xtimer_mutex_lock_timeout_short_unlocked(int argc,
                                                             char **argv);
static int cmd_test_xtimer_mutex_lock_timeout_short_locked(int argc,
                                                           char **argv);


/**
 * @brief   List of command for this application.
 */
static const shell_command_t shell_commands[] = {
    { "mutex_timeout_long_unlocked", "unlocked mutex with long timeout",
      cmd_test_xtimer_mutex_lock_timeout_long_unlocked, },
    { "mutex_timeout_long_locked", "locked mutex with long timeout",
      cmd_test_xtimer_mutex_lock_timeout_long_locked, },
    { "mutex_timeout_long_locked_low",
      "locked mutex from lower prio thread function with long timeout",
      cmd_test_xtimer_mutex_lock_timeout_low_prio_thread, },
    { "mutex_timeout_short_unlocked", "unlocked mutex with short timeout",
      cmd_test_xtimer_mutex_lock_timeout_short_unlocked, },
    { "mutex_timeout_short_locked", "locked mutex with short timeout",
      cmd_test_xtimer_mutex_lock_timeout_short_locked, },
    { NULL, NULL, NULL }
};

/**
 * @brief   thread function and stack for 
 *          cmd_test_xtimer_mutex_lock_timeout_low_prio_thread
 */

char t_stack[THREAD_STACKSIZE_MAIN];

void *thread_low_prio_test(void *arg)
{
    mutex_t *test_mutex = (mutex_t *)arg;
    msg_t msg;

    puts("THREAD low prio: start");

    mutex_lock(test_mutex);
    thread_wakeup(main_thread_pid);
    while (msg_try_receive(&msg) == -1) {}

    mutex_unlock(test_mutex);
    (void)irq_disable();
    puts("THREAD low prio: exiting low");
    msg_send_int(&msg, main_thread_pid);

    sched_task_exit();
}

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout
 *
 * the mutex is not locked before the function call and
 * the timer long. Meaning the timer will get removed
 * before triggering.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_mutex_lock_timeout_long_unlocked(int argc,
                                                            char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: xtimer mutex lock timeout");
    mutex_t test_mutex = MUTEX_INIT;

    if (xtimer_mutex_lock_timeout(&test_mutex, LONG_MUTEX_TIMEOUT) == 0) {
        /* mutex has to be locked */
        if (mutex_trylock(&test_mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error mutex not locked");
        }
    }
    else {
        puts("error: mutex timed out");
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout
 *
 * the mutex is locked before the function call and
 * the timer long. Meaning the timer will trigger
 * and remove the thread from the mutex waiting list.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_mutex_lock_timeout_long_locked(int argc,
                                                          char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: xtimer mutex lock timeout");
    mutex_t test_mutex = MUTEX_INIT;
    mutex_lock(&test_mutex);

    if (xtimer_mutex_lock_timeout(&test_mutex, LONG_MUTEX_TIMEOUT) == 0) {
        puts("Error: mutex taken");
    }
    else {
        /* mutex has to be locked */
        if (mutex_trylock(&test_mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error mutex not locked");
        }
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout
 * 
 * This function will create a new thread with lower prio.
 * than the main thread (this function should be called from
 * the main thread). The new thread will get a mutex and will
 * lock it. This function (main thread) messages the other
 * thread and calls xtimer_mutex_lock_timeout.
 * The other thread will then unlock the mutex. The main
 * thread gets the mutex and wakes up. The timer will not
 * trigger because the main threads gets the mutex.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_mutex_lock_timeout_low_prio_thread(int argc,
                                                              char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: xtimer mutex lock timeout with thread");
    mutex_t test_mutex = MUTEX_INIT;
    main_thread_pid = thread_getpid();
    int current_thread_count = sched_num_threads;
    printf("threads = %d\n", current_thread_count);
    kernel_pid_t test_thread = thread_create(t_stack, sizeof(t_stack),
                                      THREAD_PRIORITY_MAIN + 1,
                                      THREAD_CREATE_STACKTEST,
                                      thread_low_prio_test,
                                      (void *)&test_mutex,
                                      "thread_low_prio_test");

    thread_sleep();

    msg_t msg;

    puts("MAIN THREAD: calling xtimer_mutex_lock_timeout");

    msg_send(&msg, test_thread);
    if (xtimer_mutex_lock_timeout(&test_mutex, LONG_MUTEX_TIMEOUT) == 0) {
        /* mutex has to be locked */
        if (mutex_trylock(&test_mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error mutex not locked");
        }
    }
    else {
        puts("error: mutex timed out");
    }

    current_thread_count = sched_num_threads;
    printf("threads = %d\n", current_thread_count);

    /* to end the created thread */
    puts("MAIN THREAD: waiting for created thread to end");
    msg_receive(&msg);

    current_thread_count = sched_num_threads;
    printf("threads = %d\n", current_thread_count);

    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout when spinning
 *
 * The mutex is locked before the function call and
 * the timer long. Meaning the timer will trigger before
 * xtimer_mutex_lock_timeout tries to acquire the mutex.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_mutex_lock_timeout_short_locked(int argc,
                                                           char **argv)
{
    (void)argc;
    (void)argv;
    puts(
        "starting test: xtimer mutex lock timeout with short timeout and locked mutex");
    mutex_t test_mutex = MUTEX_INIT;
    mutex_lock(&test_mutex);

    if (xtimer_mutex_lock_timeout(&test_mutex, SHORT_MUTEX_TIMEOUT) == 0) {
        puts("Error: mutex taken");
    }
    else {
        /* mutex has to be locked */
        if (mutex_trylock(&test_mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error mutex not locked");
        }
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_mutex_lock_timeout when spinning
 *
 * the mutex is not locked before the function is called and
 * the timer is short. Meaning the timer will trigger before
 * xtimer_mutex_lock_timeout tries to acquire the mutex.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_mutex_lock_timeout_short_unlocked(int argc,
                                                             char **argv)
{
    (void)argc;
    (void)argv;
    puts(
        "starting test: xtimer mutex lock timeout with short timeout and unlocked mutex");
    mutex_t test_mutex = MUTEX_INIT;

    if (xtimer_mutex_lock_timeout(&test_mutex, SHORT_MUTEX_TIMEOUT) == 0) {
        /* mutex has to be locked */
        if (mutex_trylock(&test_mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error mutex not locked");
        }
    }
    else {
        puts("Error: mutex timed out");
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}


/**
 * @brief   main function starting shell
 *
 * @return 0 on success
 */
int main(void)
{
    puts("Starting shell...");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
