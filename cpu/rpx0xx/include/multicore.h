#pragma once
#include "thread.h"

void spinlock_claim_blocking(void);

void spinlock_unlock(void);

int read_cpuid(void);

void start_core(char *stackpointer, thread_task_func_t entry);
