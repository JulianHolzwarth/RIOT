/*
 * Copyright (C) 2019 Freie Universitaet Berlin,
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
 * @brief       freertos testing tool
 *
 * More detailed information about the file and the functionality implemented.
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "shell.h"

#include "freertos/FreeRTOS.h"

#include "include/semaphore_test.h"
#include "include/task_test.h"
#include "include/timer_test.h"
#include "include/queue_test.h"

/**
 * Foward declarations
 */
static int cmd_test_mutex(int argc, char **argv);
static int cmd_test_recursive_mutex(int argc, char **argv);
static int cmd_test_binary(int argc, char **argv);
static int cmd_test_counting(int argc, char **argv);
static int cmd_test_task(int argc, char **argv);
static int cmd_test_timer(int argc, char **argv);
static int cmd_test_queue(int argc, char **argv);



/**
 * @brief   List of command for this application.
 */
static const shell_command_t shell_commands[] = {
    { "mutex_semaphore", "tests freertos mutex semaphore", cmd_test_mutex, },
    { "r_mutex_semaphore", "tests freertos recursive mutex semaphore", cmd_test_recursive_mutex },
    { "binary_semaphore", "tests freertos binary semaphore",  cmd_test_binary },
    { "counting_semaphore", "tests freertos counting semaphore",  cmd_test_counting },
    { "task", "tests freertos task",  cmd_test_task },
    { "timer", "tests freertos timer",  cmd_test_timer },
    { "queue", "tests freertos queue",  cmd_test_queue },
    { NULL, NULL, NULL }
};

/**
 * @brief   shell command to test freertos mutex semaphore
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_mutex(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: mutex semaphore");
    if (semaphore_test_mutex() == pdPASS) {
        puts("OK");
    }
    else {
        puts("mutex semaphore test failed");
    }

    return 0;
}

/**
 * @brief   shell command to test freertos recursive mutex semaphore
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_recursive_mutex(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: recursive mutex semaphore");
    if (semaphore_test_recursive_mutex() == pdPASS) {
        puts("OK");
    }
    else {
        puts("recursive mutex semaphore test failed");
    }
    return 0;
}

/**
 * @brief   shell command to test freertos binary semaphore
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_binary(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: binary semaphore");
    if (semaphore_test_binary() == pdPASS) {
        puts("OK");
    }
    else {
        puts("binary semaphore test failed");
    }

    return 0;
}

/**
 * @brief   shell command to test freertos counting semaphore
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_counting(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: counting semaphore");
    if (semaphore_test_counting() == pdPASS) {
        puts("OK");
    }
    else {
        puts("counting semaphore test failed");
    }

    return 0;
}

/**
 * @brief   shell command to test freertos task
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_task(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: task");
    if (task_test_start() == pdPASS) {
        puts("OK");
    }
    else {
        puts("task test failed");
    }

    return 0;
}

/**
 * @brief   shell command to test freertos timer
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_timer(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: timer");
    if (timer_test_start() == pdPASS) {
        puts("OK");
    }
    else {
        puts("timer test failed");
    }

    return 0;
}

/**
 * @brief   shell command to test freertos queue
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return 0 on success
 */
static int cmd_test_queue(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("starting test: queue");
    if (queue_test_start() == pdPASS) {
        puts("OK");
    }
    else {
        puts("queue test failed");
    }

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
