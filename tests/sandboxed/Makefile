include ../Makefile.tests_common

USEMODULE += core_sandbox cortexm_svc
USEMODULE += syscall fmt
FEATURES_REQUIRED += cortexm_mpu
FEATURES_BLACKLIST += cortexm_fpu

BLOBS += bare/bare.bin

include $(RIOTBASE)/Makefile.include
