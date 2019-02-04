# Define the module that is built:
MODULE = cpu

# Add a list of subdirectories, that should also be built:
DIRS = periph $(RIOTCPU)/cortexm_common

# (file triggers compiler bug. see #5775)
SRC_NOLTO += vectors.c

# driverlib module folders
DIRS += $(DRIVERLIB)/driverlib
DIRS += cc26xx_rfcore

include $(RIOTBASE)/Makefile.base
