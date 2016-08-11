DEVELHELP ?= 0
include ../Makefile.tests_common

# disable assert() and all log output
CFLAGS += -DNDEBUG -DLOG_LEVEL=LOG_NONE

# disable some default modules
DISABLE_MODULE += auto_init core_msg core_thread_flags core_panic

# disable C library and uart output
DISABLE_MODULE += newlib newlib_nano uart_stdio

# enable link time optimization
LTO?=1

# adjust stack sizes to very small values
CFLAGS += -DTHREAD_STACKSIZE_MAIN=256 -DTHREAD_STACKSIZE_IDLE=128

# tweak scheduler for minimal system
CFLAGS += -DMAXTHREADS=2 -DSCHED_PRIO_LEVELS=2

include $(RIOTBASE)/Makefile.include
