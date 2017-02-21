DIRS += $(dir $(wildcard $(addsuffix /Makefile, ${USEMODULE})))

ifneq (,$(filter gsm,$(USEMODULE)))
    DIRS += gsm
endif

include $(RIOTBASE)/Makefile.base
