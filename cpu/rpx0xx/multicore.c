#include "fifo.h"
#include "multicore.h"
#include "include/vendor/RP2040.h"
#include "thread.h"
#include <stdint.h>
#include "io_reg.h"
#include <stdio.h>

int owner;

void spinlocks_init(void)
{
    /* reset all spinlocks */
    for (int i = 0; i < 32; i++) {
        spinlock_unlock(i);
    }
    owner = 0;
}

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
    fifo_drain();
}

void start_resetmode_core(void)
{
    io_reg_atomic_set(&PSM->FRCE_OFF, PSM_FRCE_OFF_proc1_Msk);
}

void stop_restmode_core(void)
{
    io_reg_atomic_clear(&PSM->FRCE_OFF, PSM_FRCE_OFF_proc1_Msk);
}

void isr_sio_proc0(void)
{
    printf("proc 0 isr, CORE %d\n", read_cpuid());
    // printf("data %d\n", fifo_has_data());
    // fifo_drain();
    // uint32_t icsr = *(volatile uint32_t *)0xe000ed04;
    // uint32_t active_vector = icsr & 0x3F; // Die unteren 6 Bitsrintf("data %d\n", fifo_has_data());
    // printf("active icsr %ld\n", active_vector);
    // uint32_t pending = *(volatile uint32_t *)0xe000e200;
    // printf("pending interrupt %ld \n", pending);
    // printf("bit 16 %d \n", (1u << 16));
    // while (*(volatile uint32_t *)(0xd0000000 + 0x50) & 1u) {
    //     (void)*(volatile uint32_t *)(0xd0000000 + 0x58);
    // }
    // (void)*(volatile uint32_t *)(0xd0000000 + 0x50);
    // __asm volatile("dsb" ::: "memory");
    // __asm volatile("isb" ::: "memory");
    // *(volatile uint32_t *)(0xd0000050) = 0xff;
}

void isr_sio_proc1(void)
{
    printf("proc 1 isr, CORE %d\n", read_cpuid());
    // printf("data %d\n", fifo_has_data());
    // fifo_drain();
    // uint32_t icsr = *(volatile uint32_t *)0xe000ed04;
    // uint32_t active_vector = icsr & 0x3F; // Die unteren 6 Bitsrintf("data %d\n", fifo_has_data());
    // printf("active icsr %ld\n", active_vector);
    // uint32_t pending = *(volatile uint32_t *)0xe000e200;
    // printf("pending interrupt %ld \n", pending);
    // printf("bit 16 %d \n", (1u << 16));
    // while (*(volatile uint32_t *)(0xd0000000 + 0x50) & 1u) {
    //     (void)*(volatile uint32_t *)(0xd0000000 + 0x58);
    // }
    // (void)*(volatile uint32_t *)(0xd0000000 + 0x50);
    // __asm volatile("dsb" ::: "memory");
    // __asm volatile("isb" ::: "memory");
    // *(volatile uint32_t *)(0xd0000050) = 0xff;
}
