
# Path containing the otawa related hg repositories
OTAWA_SOURCE_PATH?=$(PWD)
# Path of the patmos-otawa git repository
PATMOS_SOURCE_PATH?=$(PWD)
# Temporary build path
BUILD_PATH?=$(PWD)/build

# Installation path of the patmos plugin
INSTALL_PATH?=$(PWD)/../site

# Path to the otawa installation
OTAWA_PATH=$(INSTALL_PATH)
# Path to the gliss2 build
GLISS_PATH=$(OTAWA_SOURCE_PATH)/gliss2

-include Makefile.cfg


.PHONY: all checkout otawa patmos config clean install

all: patmos

#
# Targets to build OTAWA and friends itself
#

checkout:
	if [ ! -d gliss2 ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/gliss2/trunk $(OTAWA_SOURCE_PATH)/gliss2; fi
	if [ ! -d elm ];   then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/elm/trunk     $(OTAWA_SOURCE_PATH)/elm; fi
	if [ ! -d gel ];   then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/gel/trunk     $(OTAWA_SOURCE_PATH)/gel; fi
	if [ ! -d otawa ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/otawa/trunk   $(OTAWA_SOURCE_PATH)/otawa; fi
	# oRange
	if [ ! -d frontc ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/frontc/trunk $(OTAWA_SOURCE_PATH)/frontc; fi
	if [ ! -d orange ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/orange/trunk $(OTAWA_SOURCE_PATH)/orange; fi
	# Architecture plugins
	if [ ! -d ppc2 ];        then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/ppc2/trunk        $(OTAWA_SOURCE_PATH)/ppc2; fi
	if [ ! -d sparc ];       then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/sparc/trunk       $(OTAWA_SOURCE_PATH)/sparc; fi
	if [ ! -d otawa-sparc ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/otawa-sparc/trunk $(OTAWA_SOURCE_PATH)/otawa-sparc; fi
	if [ ! -d otawa-leon ];  then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/otawa-leon/trunk  $(OTAWA_SOURCE_PATH)/otawa-leon; fi
	if [ ! -d tricore ];     then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/tricore/trunk     $(OTAWA_SOURCE_PATH)/tricore; fi
	if [ ! -d tricore-1798 ];   then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/tricore-1798/trunk   $(OTAWA_SOURCE_PATH)/tricore-1798; fi
	if [ ! -d otawa-tricore ];  then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/otawa-tricore/trunk  $(OTAWA_SOURCE_PATH)/otawa-tricore; fi

otawa-config:
	mkdir -p $(BUILD_PATH)/elm
	cd $(BUILD_PATH)/elm && cmake $(OTAWA_SOURCE_PATH)/elm
	mkdir -p $(BUILD_PATH)/gel
	cd $(BUILD_PATH)/gel && cmake $(OTAWA_SOURCE_PATH)/gel
	mkdir -p $(BUILD_PATH)/otawa
	cd $(BUILD_PATH)/otawa && cmake $(OTAWA_SOURCE_PATH)/otawa
	mkdir -p $(BUILD_PATH)/otawa-sparc
	cd $(BUILD_PATH)/otawa-sparc && cmake $(OTAWA_SOURCE_PATH)/otawa-sparc
	mkdir -p $(BUILD_PATH)/otawa-leon
	cd $(BUILD_PATH)/otawa-leon && cmake $(OTAWA_SOURCE_PATH)/otawa-leon

otawa:
	# Can we build gliss2 out-of-tree?
	cd $(OTAWA_SOURCE_PATH)/gliss2 && $(MAKE)
	cd $(OTAWA_SOURCE_PATH)/ppc2 && $(MAKE) WITH_DYNLIB=1 GLISS_PREFIX=$(GLISS_PATH)
	cd $(OTAWA_SOURCE_PATH)/sparc && $(MAKE) WITH_DYNLIB=1 GLISS_PREFIX=$(GLISS_PATH)
	cd $(BUILD_PATH)/elm && $(MAKE)
	cd $(BUILD_PATH)/gel && $(MAKE)
	cd $(BUILD_PATH)/otawa && $(MAKE) 
	cd $(BUILD_PATH)/otawa-sparc && $(MAKE) 
	cd $(BUILD_PATH)/otawa-leon  && $(MAKE) 

otawa-install:
	cd $(BUILD_PATH)/otawa && $(MAKE) install
	cd $(BUILD_PATH)/otawa-sparc && $(MAKE) install
	cd $(BUILD_PATH)/otawa-leon  && $(MAKE) install


#
# Targets to build the Patmos OTAWA plugins
#

config:
	mkdir -p $(BUILD_PATH)/otawa-patmos
	cd $(BUILD_PATH)/otawa-patmos && cmake -DOTAWA_CONFIG=$(OTAWA_PATH)/bin/otawa-config -DGLISS_PATH=$(GLISS_PATH) $(PATMOS_SOURCE_PATH)/otawa-patmos
	mkdir -p $(BUILD_PATH)/patmos-wcet
	cd $(BUILD_PATH)/patmos-wcet && cmake -DOTAWA_CONFIG=$(OTAWA_PATH)/bin/otawa-config $(PATMOS_SOURCE_PATH)/patmos-wcet

patmos:
	cd $(PATMOS_SOURCE_PATH)/patmos && $(MAKE) WITH_DYNLIB=1 GLISS_PREFIX=$(GLISS_PATH)
	cd $(BUILD_PATH)/otawa-patmos && $(MAKE)
	cd $(BUILD_PATH)/patmos-wcet && $(MAKE)

clean:
	cd $(PATMOS_SOURCE_PATH)/patmos && $(MAKE) clean
	cd $(BUILD_PATH)/otawa-patmos && $(MAKE) clean
	cd $(BUILD_PATH)/patmos-wcet && $(MAKE) clean

install:
	cd $(BUILD_PATH)/otawa-patmos && $(MAKE) install
	cd $(BUILD_PATH)/patmos-wcet && $(MAKE) install

