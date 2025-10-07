#include "fifo.h"
#include "multicore.h"
#include "thread.h"
#include <stdint.h>

int owner;

void spinlock_claim_blocking(int spinlock_id)
{
    uint32_t *spinlock = (uint32_t *)(&(SIO->SPINLOCK0) + spinlock_id * sizeof(uint32_t));
    while (!*spinlock) {
    }
}

void spinlock_unlock(int spinlock_id)
{
    uint32_t *spinlock = (uint32_t *)(&(SIO->SPINLOCK0) + spinlock_id * sizeof(uint32_t));
    *spinlock = 1;
}

void spinlock_claim_with_check_blocking(void)
{
    while (!SIO->SPINLOCK1) {
    }
    if ((int)SIO->CPUID == owner - 1) {
        SIO->SPINLOCK1 = 1;
        return;
    }

    SIO->SPINLOCK1 = 1;
    while (!SIO->SPINLOCK0) {
    }
    while (!SIO->SPINLOCK1) {
    }
    owner = SIO->CPUID + 1;

    SIO->SPINLOCK1 = 1;
}

void spinlock_unlock_with_check_blocking(void)
{
    while (!SIO->SPINLOCK1) {
    }
    if ((int)SIO->CPUID == owner - 1) {
        owner = 0;
        SIO->SPINLOCK1 = 1;
        SIO->SPINLOCK0 = 1;
        return;
    }

    SIO->SPINLOCK1 = 1;
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
