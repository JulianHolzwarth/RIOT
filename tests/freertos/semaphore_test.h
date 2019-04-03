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

#ifndef SEMAPHORE_TEST_H
#define SEMAPHORE_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   tests the freertos mutex semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int semaphore_test_mutex(void);

/**
 * @brief   tests the take function for a freertos recursive mutex semaphore 
 *          NOT USED ANYMORE
 *          TODO DELETE
 *
 * @return pdPASS when the test is passed, pdFAIL otherwise
 */
int semaphore_test_recursive_mutex_take(void);

/**
 * @brief   tests the freertos recursive mutex semaphore
 *
 * @return pdPASS when the test is passed, pdFAIL otherwise
 */
int semaphore_test_recursive_mutex(void);
/**
 * @brief   tests the freertos binary semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int semaphore_test_binary(void);

/**
 * @brief   tests the freertos counting semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int semaphore_test_counting(void);

#ifdef __cplusplus
}
#endif

#endif /* SEMAPHORE_TEST_H */
