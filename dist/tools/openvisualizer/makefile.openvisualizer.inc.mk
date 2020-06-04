.PHONY: openv-clean openv-setroot openv-term openv-termroot

# Use a single board with openv
# ===================================
#
# OpenVisualizer runs on port 9000 by default, if that ports conflicts or
# multiple instances are spawned you will need to specify the port, eg.
#   * `OPENV_FLAGS='--port=9001`
#
# Not all logs for openvisualizer are piped to the terminal, more detailed logs
# are stored in $(BINDIR)/openv-server.log
#
# More info at https://github.com/fjmolinas/openvisualizer/blob/develop_SW-318-RIOT/README.md
#
# Supported:
#  * openv-term
#  * openv-termroot
#  * openv-termtun (will start openv as sudo)
#  * openv-setroot
#  * openv-clean
#
# Prerequisites
# -------------
#
# * Install openvisualizer:
#   * git clone -b develop_SW-318-RIOT https://github.com/fjmolinas/openvisualizer.git
#   * cd openvisualizer
#   * pip2 install .
#
# * If using iotlab nodes, pre-requisites in makefile.iotlab.single.inc.mk
#
# * For `openv-termtun` it will require root must be able to ssh into iotlab
#

# Use full path in case it needs to be run with sudo
OPENV_SERVER_PATH := $(shell which openv-server)
OPENV_CLIENT_PATH := $(shell which openv-client)

# Openvisualizer requires to know where openwsn-fw is located
OPENV_OPENWSN_FW_PATH ?= --fw-path=$(BINDIRBASE)/pkg/$(BOARD)/openwsn-fw
OPENV_DEFAULT_FLAGS += $(OPENV_OPENWSN_FW_PATH)

OPENV_DEFAULT_FLAGS ?=

ifneq (,$(IOTLAB_NODE))
  OPENV_MOTE ?= $(IOTLAB_NODE)
  OPENV_DEFAULT_FLAGS += --iotlab-motes=$(IOTLAB_NODE)
else
  OPENV_MOTE += $(PORT)
  OPENV_DEFAULT_FLAGS += --port-mask=$(OPENV_MOTE) --baudrate=$(BAUD)
endif

# Use modified logging configuration
OPENV_LOG_CONFIG = $(BINDIR)/logging.conf
OPENV_LOG_FILE = $(BINDIR)/openv-server.log
OPENV_DEFAULT_FLAGS += --lconf=$(OPENV_LOG_CONFIG)

$(OPENV_LOG_CONFIG): $(LAST_MAKEFILEDIR)/logging.conf
	$(Q)cp $^ $@.tmp
	$(Q)sed -i 's#LOG_PATH#'"$(BINDIR)"'#g' $@.tmp
	$(Q)mv $@.tmp $@

# Start tun interface
ifneq (,$(filter openv-termtun,$(MAKECMDGOALS)))
  OPENV_DEFAULT_FLAGS += --opentun
endif

# Optional flags to pass through command line
OPENV_FLAGS ?=

openv-term: $(OPENV_LOG_CONFIG)
openv-term: $(TERMDEPS)
	$(Q)$(OPENV_SERVER_PATH) $(OPENV_DEFAULT_FLAGS)$(OPENV_FLAGS)

openv-termroot: $(OPENV_LOG_CONFIG)
openv-termroot: $(TERMDEPS)
	$(Q)$(OPENV_SERVER_PATH) $(OPENV_DEFAULT_FLAGS) $(OPENV_FLAGS) --root=$(OPENV_MOTE)

openv-termtun: $(OPENV_LOG_CONFIG)
openv-termtun:  $(TERMDEPS)
	$(Q)sudo $(OPENV_SERVER_PATH) $(OPENV_DEFAULT_FLAGS) $(OPENV_FLAGS) --root=$(OPENV_MOTE)

openv-setroot:
	$(Q)$(OPENV_CLIENT_PATH) $(OPENV_OPENWSN_FW_PATH) $(OPENV_FLAGS) root=$(OPENV_MOTE)

openv-clean:
	$(Q)rm -rf $(OPENV_LOG_CONFIG)
	$(Q)rm -rf $(OPENV_LOG_FILE)
