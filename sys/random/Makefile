ifeq (,$(filter prng_%,$(USEMODULE)))
    USEMODULE += prng_tinymt32
endif
ifneq (,$(filter prng_mersenne,$(USEMODULE)))
    SRC += mersenne.c
endif
ifneq (,$(filter prng_minstd,$(USEMODULE)))
    SRC += minstd.c
endif
ifneq (,$(filter prng_musl_lcg,$(USEMODULE)))
    SRC += musl_lcg.c
endif
ifneq (,$(filter prng_tinymt32,$(USEMODULE)))
    SRC += tinymt32.c
    DIRS += tinymt32
endif

include $(RIOTBASE)/Makefile.base
