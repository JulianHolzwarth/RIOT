#pragma once
#include <stdint.h>

/**
 * @brief   Returns 1 if data in fifo
 *
 * @retval 1  has data in fifo 
 * @retval 0  has no data in fifo 
 */
int fifo_has_data(void);

/**
 * @brief   Returns 1 if fifo can be written to
 *
 * @retval 1  can write in fifo 
 * @retval 0  cannot write in fifo 
 */
int fifo_can_write(void);

/**
 * @brief   Drains fifo
 */
void fifo_drain(void);

/**
 * @brief   Writes data to fifo while blocking
 *          does not return until written to fifo
 *
 * @param data   data to write to fifo
 */
void fifo_write_blocking(uint32_t data);

/**
 * @brief   Reads fifo and returns one value
 *
 * @return   data read from fifo  
 */
uint32_t fifo_read(void);

/**
 * @brief   Waits until fifo has data and reads one value
 *
 * @return   data read from fifo 
 */
uint32_t fifo_read_blocking(void);

/**
 * @brief   Writes to fifo and checks if other core returns the same value
 *          uses blocking write and read
 *          drains fifo
 *
 * @param data   data to write to fifo
 * @return       returns if the value the other core wrote to fifo as response
 *               is equal to data
 * @retval true  return is equal to data
 * @retval false retrun is not equal to data
 */
int fifo_write_with_check(uint32_t data);

/**
 * @brief   Writes buffer to fifo with check,
 *          is blocking and drains fifo,
 *          writes buffer to fifo until other core respones
 *          with the same values
 *
 * @param buffer        pointer to values that will be written to fifo
 * @param buffer_size   size of buffer
 */
void fifo_write_buffer_blocking(const uint32_t *buffer, const int buffer_size);
