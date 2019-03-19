RIOTBASE ?= $(CURDIR)
USEMODULE ?= $(USEMODULE)
PRE := $(USEMODULE)
include Makefile.dep

all:
	@for i in $(filter-out $(PRE),$(USEMODULE) $(FEATURES_REQUIRED)); do echo $$i; done
