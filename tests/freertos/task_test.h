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

#ifndef TASK_TEST_H
#define TASK_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   tests the freertos task
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int task_test_start(void);

/**
 * @brief   thread for the test
 *
 * @param[in] parameter  input of the thread
 * 
 * @return NULL
 */
void task_test_thread(void * pvParameters);

#ifdef __cplusplus
}
#endif

#endif /* TASK_TEST_H */
