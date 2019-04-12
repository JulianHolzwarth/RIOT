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

#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"

#include "queue_test.h"

/* to test */
static mutex_t test_mutex;
static mutex_t thread1_done_mutex;
static mutex_t thread2_done_mutex;

/* Define the data type that will be queued. */
typedef struct {
    char ucMessageID;
    char ucData[ 20 ];
} Test_MESSAGE;

/* Define the queue parameters. */
#define QUEUE_LENGTH 5
#define QUEUE_ITEM_SIZE sizeof(Test_MESSAGE)

static void queue_test_thread1(void *pvParameters)
{
    QueueHandle_t testing_queue = (QueueHandle_t)pvParameters;
    Test_MESSAGE message_receive;

    for (size_t i = 0; i < 20; i++) {
        while (uxQueueMessagesWaiting(testing_queue) == 0) {
            vTaskDelay(5);
        }
        if (xQueueReceive(testing_queue, &message_receive, 0) == errQUEUE_EMPTY) {
            puts("queue could not revceive");
            mutex_trylock(&test_mutex);
            break;
        }
        if (message_receive.ucMessageID != (char)i) {
            puts("queue received the wrong data");
            mutex_trylock(&test_mutex);
            break;
        }

    }
    mutex_trylock(&thread1_done_mutex);
}

static void queue_test_thread2(void *pvParameters)
{
    QueueHandle_t testing_queue = (QueueHandle_t)pvParameters;
    Test_MESSAGE message_send;

    for (size_t i = 0; i < 20; i++) {
        message_send.ucMessageID = i;
        if (xQueueSend(testing_queue, &message_send, 0) != pdPASS) {
            puts("could not send");
            mutex_trylock(&test_mutex);
            break;
        }
        vTaskDelay(1);

    }
    mutex_trylock(&thread2_done_mutex);
}


/**
 * @brief   tests the freertos queue
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int queue_test_start(void)
{
    bool test_result = pdPASS;

    mutex_init(&test_mutex);
    mutex_init(&thread1_done_mutex);
    mutex_init(&thread2_done_mutex);
    QueueHandle_t testing_queue;
    testing_queue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    if (testing_queue == NULL) {
        puts("error in queue creation");
        test_result = pdFAIL;
        return test_result;
    }
    TaskHandle_t xHandle1;
    TaskHandle_t xHandle2;
    xTaskCreate( queue_test_thread1, "testing task 1", THREAD_STACKSIZE_DEFAULT, (void *)testing_queue, 1, &xHandle1 );
    xTaskCreate( queue_test_thread2, "testing task 2", THREAD_STACKSIZE_DEFAULT, (void *)testing_queue, 1, &xHandle2 );

    while ((&thread1_done_mutex)->queue.next == NULL || (&thread2_done_mutex)->queue.next == NULL) {
        vTaskDelay(10);
    }

    vTaskDelete(xHandle1);
    vTaskDelete(xHandle2);
    vQueueDelete(testing_queue);
    if ((&test_mutex)->queue.next != NULL) {
        puts("error in threads");
        test_result = pdFAIL;
    }
    if (test_result == pdFAIL) {
        return pdFAIL;
    }
    return pdPASS;
}
