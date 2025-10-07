#pragma once
#include "thread.h"

typedef void *(*thread_task_func_t)(void *arg);
void spinlock_claim_blocking(int spinlock_id);

void spinlock_unlock(int spinlock_id);

void spinlock_claim_with_check_blocking(void);

void spinlock_unlock_with_check_blocking(void);

int read_cpuid(void);

void start_core(char *stackpointer, thread_task_func_t entry);
