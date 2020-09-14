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
 * @brief msg untested lines
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "msg.h"
#include "thread.h"
#include "irq.h"
#include "xtimer.h"

#define MSG_QUEUE_LENGTH                (8)

static char thread1_stack[THREAD_STACKSIZE_MAIN];
static char thread_dead_stack[THREAD_STACKSIZE_IDLE];

static msg_t msg_queue[MSG_QUEUE_LENGTH];
static kernel_pid_t pid_main, thread1_pid, pid_dead_thread;
static int error;
static msg_t interupt_msg, temp_msg;
static msg_t interupt_msg2;

static void callback_function(void *arg)
{
    (void)arg;
    puts("cb 1");
    if (!irq_is_in()) {
        puts("ERROR not in irq");
        error = 1;
    }
    msg_t msg;
    if (msg_send(&msg, KERNEL_PID_LAST) != -1) {
        puts("ERROR to undefined");
        error = 1;
    }
    if (msg_send(&msg, pid_main) != 0) {
        puts("ERROR in send");
        error = 1;
    }
    if (msg_try_send(&msg, pid_main) != 0) {
        puts("ERROR in try send");
        error = 1;
    }
    thread_wakeup(pid_main);
}

static void callback_function2(void *arg)
{
    (void)arg;
    puts("cb 2");
    /* idle thread (not reply blocked)*/
    temp_msg.sender_pid = pid_dead_thread;
    if (msg_reply_int(&temp_msg, &interupt_msg2) == 1) {
        puts("ERROR");
    }
    interupt_msg.sender_pid = pid_main;
    if (msg_reply_int(&interupt_msg, &interupt_msg2) != 1) {
        puts("ERROR");
    }
}

static void *thread1(void *args)
{
    (void)args;
    puts("t 1");
    msg_t thread_msg;
    /* idle thread (not reply blocked)*/
    temp_msg.sender_pid = pid_dead_thread;
    if (msg_reply(&temp_msg, &thread_msg) == 1) {
        puts("ERROR");
    }
    msg_t msg_queue_thread[MSG_QUEUE_LENGTH];
    msg_init_queue(msg_queue_thread, MSG_QUEUE_LENGTH);


    thread_sleep();
    return NULL;
}

static void *dead_thread(void *args)
{
    (void)args;
    thread_sleep();
    return NULL;
}

int main(void)
{

    pid_dead_thread = thread_create(thread_dead_stack, THREAD_STACKSIZE_IDLE,
                                    THREAD_PRIORITY_MAIN + 1,
                                    THREAD_CREATE_SLEEPING, dead_thread, NULL,
                                    "thread1");
    error = 0;
    msg_t msg, msg2;

    if (msg_try_receive(&msg) != -1) {
        puts("ERROR");
        error = 1;
    }
    pid_main = thread_getpid();
    /* test in irq */
    xtimer_t t;
    t.callback = callback_function;
    xtimer_set64(&t, 1000);
    puts("timer");
    puts("sleep");
    thread_sleep();

    if (msg_try_send(&msg, pid_main) != 0) {
        puts("ERROR try send without queue");
        error = 1;
    }
    if (msg_send(&msg, pid_main) != 0) {
        puts("ERROR send without queue");
        error = 1;
    }
    msg_queue_print();


    msg_init_queue(msg_queue, MSG_QUEUE_LENGTH);

    msg.type = 1;
    msg_send(&msg, thread_getpid());
    msg_receive(&msg2);
    if (msg.type != msg2.type) {
        puts("ERROR");
        error = 1;
    }
    msg.type = 2;
    msg_try_send(&msg, pid_main);
    msg_receive(&msg2);
    if (msg.type != msg2.type) {
        puts("ERROR");
        error = 1;
    }
    if (msg_send(&msg, KERNEL_PID_LAST) != -1) {
        puts("ERROR");
        error = 1;
    }

    t.callback = callback_function2;
    xtimer_set64(&t, (uint64_t)1000);
    msg_send_receive(&msg, &msg2, pid_dead_thread);
    msg_queue_print();

    thread1_pid = thread_create(thread1_stack, THREAD_STACKSIZE_MAIN,
                                THREAD_PRIORITY_MAIN - 1,
                                0, thread1, NULL, "thread1");


    if (error == 1) {
        puts("FAILURE");
        return 1;
    }
    puts("[SUCCESS]");
    return 0;
}
