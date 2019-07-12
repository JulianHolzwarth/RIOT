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
 * @brief       testing xtimer_rmutex_lock_timeout function
 *
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "xtimer.h"
#include "thread.h"
#include "msg.h"
#include "irq.h"

/* timeout at one millisecond (1000 us) to make sure it does not spin. */
#define LONG_RMUTEX_TIMEOUT 1000

/* timeout smaller than XTIMER_BACKOFF to make sure it spins. */
#define SHORT_RMUTEX_TIMEOUT ((1 << XTIMER_SHIFT) + 1)

/**
 * Foward declarations
 */
static int cmd_test_xtimer_rmutex_lock_timeout_long_unlocked(int argc,
                                                             char **argv);
static int cmd_test_xtimer_rmutex_lock_timeout_long_locked(int argc,
                                                           char **argv);
static int cmd_test_xtimer_rmutex_lock_timeout_long_lower_lock_unlocked(
    int argc, char **argv);

static int cmd_test_xtimer_rmutex_lock_timeout_short_unlocked(int argc,
                                                              char **argv);
static int cmd_test_xtimer_rmutex_lock_timeout_short_locked(int argc,
                                                            char **argv);


/**
 * @brief   List of command for this application.
 */
static const shell_command_t shell_commands[] = {
    { "rmutex_timeout_long_unlocked", "unlocked rmutex with long timeout",
      cmd_test_xtimer_rmutex_lock_timeout_long_unlocked, },
    { "rmutex_timeout_long_locked", "locked rmutex with long timeout",
      cmd_test_xtimer_rmutex_lock_timeout_long_locked, },
    { "rmutex_timeout_long_lower_unlock_locked",
      "unlocking locked rmutex with lower prio thread while in function with long timeout",
      cmd_test_xtimer_rmutex_lock_timeout_long_lower_lock_unlocked, },

    { "rmutex_timeout_short_unlocked", "unlocked rmutex with short timeout",
      cmd_test_xtimer_rmutex_lock_timeout_short_unlocked, },
    { "rmutex_timeout_short_locked", "locked rmutex with short timeout",
      cmd_test_xtimer_rmutex_lock_timeout_short_locked, },
    { NULL, NULL, NULL }
};

/* main Thread PID */
kernel_pid_t main_thread_pid;

char t2_stack[THREAD_STACKSIZE_MAIN];

void *lock_mutex_thread(void *arg)
{
    rmutex_t *test_rmutex = (rmutex_t *)arg;

    rmutex_lock(test_rmutex);
    sched_task_exit();
}

void *test_thread(void *arg)
{
    rmutex_t *test_rmutex = (rmutex_t *)arg;
    msg_t msg;

    rmutex_lock(test_rmutex);
    puts("THREAD low prio: locked and waiting");
    thread_wakeup(main_thread_pid);
    while (msg_try_receive(&msg) != 1) {}

    puts("THREAD low prio: unlocking");
    rmutex_unlock(test_rmutex);

    (void)irq_disable();
    puts("THREAD low prio: exiting");
    msg_send_int(&msg, main_thread_pid);

    sched_task_exit();
}

/**
 * @brief   shell command to test xtimer_rmutex_lock_timeout
 *
 * the rmutex is not locked before the function call and
 * the timer long. Meaning the timer will get removed
 * before triggering.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_rmutex_lock_timeout_long_unlocked(int argc,
                                                             char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: xtimer rmutex lock timeout");
    rmutex_t test_rmutex = RMUTEX_INIT;

    if (xtimer_rmutex_lock_timeout(&test_rmutex, LONG_RMUTEX_TIMEOUT) == 0) {
        if (atomic_load_explicit(&test_rmutex.owner,
                                 memory_order_relaxed) == thread_getpid() &&
            test_rmutex.refcount == 1 &&
            mutex_trylock(&test_rmutex.mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error rmutex wrong variables in struct");
        }
    }
    else {
        puts("error: rmutex timed out");
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_rmutex_lock_timeout
 *
 * the rmutex is locked before the function call and
 * the timer long. Meaning the timer will trigger
 * and remove the thread from the rmutex waiting list.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_rmutex_lock_timeout_long_locked(int argc,
                                                           char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: xtimer rmutex lock timeout");
    rmutex_t test_rmutex = RMUTEX_INIT;

    kernel_pid_t second_t_pid = thread_create( t2_stack, sizeof(t2_stack),
                                               THREAD_PRIORITY_MAIN - 1,
                                               THREAD_CREATE_STACKTEST,
                                               lock_mutex_thread,
                                               (void *)&test_rmutex,
                                               "lock_thread");



    if (xtimer_rmutex_lock_timeout(&test_rmutex, LONG_RMUTEX_TIMEOUT) == 0) {
        puts("Error: rmutex taken");
    }
    else {
        if (atomic_load_explicit(&test_rmutex.owner,
                                 memory_order_relaxed) == second_t_pid &&
            test_rmutex.refcount == 1 &&
            mutex_trylock(&test_rmutex.mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error rmutex wrong variables in struct");
        }
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_rmutex_lock_timeout
 *
 * the rmutex is locked before the function call,
 * the timer long and getting unlocked (from lower prio thread)
 * before the timeout.
 * Meaning the timer will get removed before triggering.
 * But not immediately.
 * 
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_rmutex_lock_timeout_long_lower_lock_unlocked(
    int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: xtimer rmutex lock timeout");
    main_thread_pid = thread_getpid();
    rmutex_t test_rmutex = RMUTEX_INIT;

    kernel_pid_t second_t_pid = thread_create( t2_stack, sizeof(t2_stack),
                                               THREAD_PRIORITY_MAIN + 1,
                                               THREAD_CREATE_STACKTEST,
                                               test_thread,
                                               (void *)&test_rmutex,
                                               "test_thread");

    puts("MAIN THREAD: created thread");
    thread_sleep();
    msg_t msg;
    msg_send(&msg, second_t_pid);
    puts("MAIN THREAD: locking");
    if (xtimer_rmutex_lock_timeout(&test_rmutex, LONG_RMUTEX_TIMEOUT) == 0) {
        if (atomic_load_explicit(&test_rmutex.owner,
                                 memory_order_relaxed) == thread_getpid() &&
            test_rmutex.refcount == 1 &&
            mutex_trylock(&test_rmutex.mutex) == 0) {
            puts("OK");
        }
        else {
            puts("MAIN THREAD: error rmutex wrong variables in struct");
        }
    }
    else {
        puts("error: rmutex timed out");
    }

    /* to end the created thread */
    puts("MAIN THREAD: waiting for created thread to end");
    msg_receive(&msg);

    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_rmutex_lock_timeout when spinning
 *
 * The rmutex is locked before the function call and
 * the timer long. Meaning the timer will trigger before
 * xtimer_rmutex_lock_timeout tries to acquire the rmutex.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_rmutex_lock_timeout_short_locked(int argc,
                                                            char **argv)
{
    (void)argc;
    (void)argv;
    puts(
        "starting test: xtimer rmutex lock timeout with short timeout and locked rmutex");
    rmutex_t test_rmutex = RMUTEX_INIT;

    kernel_pid_t second_t_pid = thread_create( t2_stack, sizeof(t2_stack),
                                               THREAD_PRIORITY_MAIN - 1,
                                               THREAD_CREATE_STACKTEST,
                                               lock_mutex_thread,
                                               (void *)&test_rmutex,
                                               "lock_thread");



    if (xtimer_rmutex_lock_timeout(&test_rmutex, SHORT_RMUTEX_TIMEOUT) == 0) {
        puts("Error: rmutex taken");
    }
    else {
        if (atomic_load_explicit(&test_rmutex.owner,
                                 memory_order_relaxed) == second_t_pid &&
            test_rmutex.refcount == 1 &&
            mutex_trylock(&test_rmutex.mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error rmutex wrong variables in struct");
        }
    }
    /* to make the test easier to read */
    printf("\n");

    return 0;
}

/**
 * @brief   shell command to test xtimer_rmutex_lock_timeout when spinning
 *
 * the rmutex is not locked before the function is called and
 * the timer is short. Meaning the timer will trigger before
 * xtimer_rmutex_lock_timeout tries to acquire the rmutex.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_xtimer_rmutex_lock_timeout_short_unlocked(int argc,
                                                              char **argv)
{
    (void)argc;
    (void)argv;
    puts(
        "starting test: xtimer rmutex lock timeout with short timeout and unlocked rmutex");
    rmutex_t test_rmutex = RMUTEX_INIT;

    if (xtimer_rmutex_lock_timeout(&test_rmutex, SHORT_RMUTEX_TIMEOUT) == 0) {
        if (atomic_load_explicit(&test_rmutex.owner,
                                 memory_order_relaxed) == thread_getpid() &&
            test_rmutex.refcount == 1 &&
            mutex_trylock(&test_rmutex.mutex) == 0) {
            puts("OK");
        }
        else {
            puts("error rmutex wrong variables in struct");
        }
    }
    else {
        puts("Error: rmutex timed out");
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
