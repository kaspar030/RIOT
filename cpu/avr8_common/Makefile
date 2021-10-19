# define the module that is build (not strictly necessary)
MODULE = avr8_common

# add a list of subdirectories, that should also be build
ifneq (libc,$(RIOT_LIBC))
  DIRS += avr_libc_extra
endif

include $(RIOTBASE)/Makefile.base
