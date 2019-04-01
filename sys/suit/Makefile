SUBMODULES := 1

# don't complain about missing submodule .c file.
# necessary to not fail for suit_v1_*.
SUBMODULES_NOFORCE := 1

ifneq (, $(filter suit_v1, $(USEMODULE)))
  DIRS += v1
endif
ifneq (, $(filter suit_v4, $(USEMODULE)))
  DIRS += v4
endif
ifneq (, $(filter suit_minimal, $(USEMODULE)))
  DIRS += minimal
endif

include $(RIOTBASE)/Makefile.base
