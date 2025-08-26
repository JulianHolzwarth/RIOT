#include "fifo.h"
#define ENABLE_DEBUG 0
#include "debug.h"

int fifo_has_data(void)
{
    return SIO->FIFO_ST & 0x00000001;
}

int fifo_can_write(void)
{
    return SIO->FIFO_ST & 0x00000002;
}

void fifo_drain(void)
{
    while (fifo_has_data()) {
        (void)SIO->FIFO_RD;
    }
}

void fifo_write_blocking(uint32_t data)
{
    DEBUG("write FIFO");
    while (!fifo_can_write()) {
    }
    SIO->FIFO_WR = data;
    __SEV();
}

uint32_t fifo_read(void)
{
    if (fifo_has_data()) {
        return SIO->FIFO_RD;
    }
    DEBUG("no data");
    return 0;
}

uint32_t fifo_read_blocking(void)
{
    DEBUG("read blocking");
    uint32_t fifo_st_reg = SIO->FIFO_ST;
    DEBUG("fifo st register: %08" PRIx32 "\n", fifo_st_reg);
    while (!fifo_has_data()) {
    }
    uint32_t data = SIO->FIFO_RD;
    return data;
}

int fifo_write_with_check(uint32_t data)
{
    if (data == 0) {
        fifo_drain();
        DEBUG("FIFO drain");
        __SEV();
    }
    fifo_write_blocking(data);
    uint32_t return_data = fifo_read_blocking();
    DEBUG("return data: %p\n data: %p\n", (void *)return_data, (void *)data);
    if (return_data == data) {
        DEBUG("true in write");
        return true;
    }
    DEBUG("false in write");
    return false;
}

void fifo_write_buffer_blocking(const uint32_t *buffer, const int buffer_size)
{
    for (int i = 0; i < buffer_size; i++) {
        if (fifo_write_with_check(buffer[i]) == false) {
            /* retry writing entire buffer on fail */
            i = 0;
        }
    }
}
