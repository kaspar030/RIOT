DEVELHELP ?= 0
include ../Makefile.tests_common

-include $(RIOTBOARD)/$(BOARD)/Makefile.features

BOARD_INSUFFICIENT_MEMORY := airfy-beacon \
                             arduino-duemilanove \
                             arduino-mega2560 \
                             arduino-mkr1000 \
                             arduino-mkrfox1200 \
                             arduino-mkrzero \
                             arduino-uno \
                             arduino-zero \
                             avsextrem \
                             b-l072z-lrwan1 \
                             bluepill \
                             calliope-mini \
                             cc2538dk \
                             cc2650-launchpad \
                             cc2650stk \
                             chronos \
                             ek-lm4f120xl \
                             feather-m0 \
                             limifrog-v1 maple-mini \
                             mbed_lpc1768 \
                             mega-xplained \
                             microbit \
                             msb-430 \
                             msb-430h \
                             msba2 \
                             nrf51dongle \
                             nrf6310 \
                             nucleo32-f031 \
                             nucleo32-f042 \
                             nucleo32-f303 \
                             nucleo32-l031 \
                             nucleo32-l432 \
                             nucleo-f030 \
                             nucleo-f070 \
                             nucleo-f072 \
                             nucleo-f091 \
                             nucleo-f103 \
                             nucleo-f302 \
                             nucleo-f334 \
                             nucleo-f410 \
                             nucleo-l053 \
                             nucleo-l073 \
                             nucleo-l433rc \
                             nz32-sc151 \
                             opencm904 \
                             openmote \
                             openmote-cc2538 \
                             pba-d-01-kw2x \
                             remote-pa \
                             remote-reva \
                             remote-revb \
                             saml21-xpro \
                             samd21-xpro \
                             samr21-xpro \
                             seeeduino_arch-pro \
                             slwstk6220a \
                             sodaq-autonomo \
                             sodaq-explorer \
                             spark-core \
                             stm32f0discovery \
                             stm32f3discovery \
                             teensy31 \
                             telosb \
                             waspmote-pro \
                             wsn430-v1_3b \
                             wsn430-v1_4 \
                             yunjia-nrf51822 z1

USEMODULE += embunit

ifeq (, $(filter tests-%, $(MAKECMDGOALS)))
  # the $(dir) Makefile function leaves a trailing slash after the directory
  # name, therefore we use patsubst instead.
  UNIT_TESTS := $(patsubst %/Makefile,%,$(wildcard tests-*/Makefile))
else
  UNIT_TESTS := $(filter tests-%, $(MAKECMDGOALS))
endif

ifneq ($(BOARD),native)
  DISABLE_TESTS += tests-relic
endif

ifneq (,$(filter arch_arm7 arch_avr8 arch_msp430, $(FEATURES_PROVIDED)))
  DISABLE_TESTS += tests-cpp_%
endif

ifneq (,$(filter arch_msp430, $(FEATURES_PROVIDED)))
  DISABLE_TESTS += tests-spiffs
endif

UNIT_TESTS := $(filter-out $(DISABLE_TESTS), $(UNIT_TESTS))

ifneq (,$(filter tests-cpp_%, $(UNIT_TESTS)))
  # We need to tell the build system to use the C++ compiler for linking
  export FEATURES_REQUIRED += cpp
  export CPPMIX := 1
endif

ifneq (,$(filter arch_avr8, $(FEATURES_PROVIDED)))
  LARGE_STACK_TESTS += tests-qDSA
endif

LARGE_STACK_TESTS += tests-hacl
LARGE_STACK_TESTS += tests-libcose
LARGE_STACK_TESTS += tests-tweetnacl
ifneq (,$(filter $(LARGE_STACK_TESTS), $(UNIT_TESTS)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(4*THREAD_STACKSIZE_DEFAULT+THREAD_EXTRA_STACKSIZE_PRINTF\)
endif

DISABLE_MODULE += auto_init

# Pull in `Makefile.include`s from the test suites:
-include $(UNIT_TESTS:%=$(RIOTBASE)/tests/unittests/%/Makefile.include)

DIRS += $(UNIT_TESTS)
BASELIBS += $(UNIT_TESTS:%=$(BINDIR)/%.a)

INCLUDES += -I$(RIOTBASE)/tests/unittests/common

# list of boards to run CI tests on
TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

.PHONY: $(UNIT_TESTS)

all:

info-unittests:
	@echo $(UNIT_TESTS)

$(UNIT_TESTS): all

charCOMMA := ,

ifeq (, $(UNIT_TESTS))
  CFLAGS += -DNO_TEST_SUITES
  $(warning There was no test suite specified!)
else
  CFLAGS += -DTEST_SUITES='$(subst $() $(),$(charCOMMA),$(UNIT_TESTS:tests-%=%))'
endif

test:
	./tests/01-run.py
