include ../Makefile.core_common

USEMODULE += printf_float
USEMODULE += ztimer_usec

# native has known issues: the context switch via glibc's setcontext()
# apparently doesn't properly save and restore the FPU state. This results in
# occasionally wrong results (often nan) being printed for the same calculation
TEST_ON_CI_BLACKLIST += native32 native64

#DISABLE_MODULE += cortexm_fpu

include $(RIOTBASE)/Makefile.include
