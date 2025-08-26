#include "fifo.h"
#include "multicore.h"
#include "thread.h"
#include <stdint.h>

void spinlock_claim_blocking(void)
{
    while (!SIO->SPINLOCK0) {
    }
}

void spinlock_unlock(void)
{
    SIO->SPINLOCK0 = 1;
}

int read_cpuid(void)
{
    return SIO->CPUID;
}

void start_core(char *stackpointer, thread_task_func_t entry)
{
    const uint32_t cmd_sequence[] = { 0, 0, 1, SCB->VTOR, (uint32_t)stackpointer, (uint32_t)entry };

    // start cpu 1
    fifo_write_buffer_blocking(cmd_sequence, 6);
}
