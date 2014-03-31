
BUILD_PATH?=$(PWD)/build
INSTALL_PATH=$(PWD)/../local

OTAWA_PATH=$(PWD)/../site
GLISS_PATH=$(PWD)/../gliss2

-include Makefile.cfg

.PHONY: all checkout otawa patmos config clean install

ELM_SOURCE_PATH=$(PWD)/elm
GEL_SOURCE_PATH=$(PWD)/gel
OTAWA_SOURCE_PATH=$(PWD)/otawa
PATMOS_SOURCE_PATH=$(PWD)/patmos
OTAWA_PATMOS_SOURCE_PATH=$(PWD)/otawa-patmos
PATMOS_PROC_SOURCE_PATH=$(PWD)/proc

all: patmos

checkout:
	if [ ! -d gliss2 ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/gliss2/trunk gliss2; fi
	if [ ! -d elm ];   then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/elm/trunk elm; fi
	if [ ! -d gel ];   then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/gel/trunk gel; fi
	if [ ! -d otawa ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/otawa/trunk otawa; fi
	# oRange
	if [ ! -d frontc ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/frontc/trunk frontc; fi
	if [ ! -d orange ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/orange/trunk orange; fi
	# Architecture plugins
	if [ ! -d ppc2 ];        then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/ppc2/trunk ppc2; fi
	if [ ! -d sparc ];       then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/sparc/trunk sparc; fi
	if [ ! -d otawa-sparc ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/otawa-sparc/trunk otawa-sparc; fi
	if [ ! -d otawa-leon ];  then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/otawa-leon/trunk otawa-leon; fi

otawa-config:
	mkdir -p $(BUILD_PATH)/elm
	cd $(BUILD_PATH)/elm && cmake $(ELM_SOURCE_PATH)
	mkdir -p $(BUILD_PATH)/gel
	cd $(BUILD_PATH)/gel && cmake $(GEL_SOURCE_PATH)
	mkdir -p $(BUILD_PATH)/otawa
	cd $(BUILD_PATH)/otawa && cmake $(OTAWA_SOURCE_PATH)

otawa:
	# Can we build gliss2 out-of-tree??
	cd gliss2 && $(MAKE)
	cd $(BUILD_PATH)/elm && $(MAKE)
	cd $(BUILD_PATH)/gel && $(MAKE)
	cd $(BUILD_PATH)/otawa && $(MAKE) 

otawa-install:
	cd $(BUILD_PATH)/otawa && $(MAKE) install

config:
	mkdir -p $(BUILD_PATH)/otawa-patmos
	cd $(BUILD_PATH)/otawa-patmos && cmake -DOTAWA_CONFIG=$(OTAWA_PATH)/bin/otawa-config -DGLISS_PATH=$(GLISS_PATH) $(OTAWA_PATMOS_SOURCE_PATH)
	mkdir -p $(BUILD_PATH)/patmos-proc
	cd $(BUILD_PATH)/patmos-proc && cmake -DOTAWA_CONFIG=$(OTAWA_PATH)/bin/otawa-config $(PATMOS_PROC_SOURCE_PATH)

patmos:
	cd patmos && $(MAKE) WITH_DYNLIB=1 GLISS_PREFIX=$(GLISS_PATH)
	cd build/otawa-patmos && $(MAKE)
	cd build/patmos-proc && $(MAKE)

clean:
	cd patmos && $(MAKE) clean
	cd build/otawa-patmos && $(MAKE) clean
	cd build/patmos-proc && $(MAKE) clean

install:
	cd build/otawa-patmos && $(MAKE) install
	cd build/patmos-proc && $(MAKE) install
