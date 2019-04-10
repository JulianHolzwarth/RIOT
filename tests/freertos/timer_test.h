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

#ifndef TIMER_TEST_H
#define TIMER_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   tests the freertos timer
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int timer_test_start(void);


#ifdef __cplusplus
}
#endif

#endif /* TIMER_TEST_H */
