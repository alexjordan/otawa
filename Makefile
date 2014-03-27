
OTAWA_PATH=$(PWD)/../site
GLISS_PATH=$(PWD)/../gliss2

-include Makefile.cfg

.PHONY: all config clean install

all:
	cd patmos && $(MAKE)
	cd build/otawa-patmos && $(MAKE)
	cd build/proc && $(MAKE)

config:
	mkdir -p build/otawa-patmos
	cd build/otawa-patmos && cmake -DOTAWA_CONFIG=$(OTAWA_PATH)/bin/otawa-config -DGLISS_PATH=$(GLISS_PATH) ../../otawa-patmos
	mkdir -p build/proc
	cd build/proc && cmake -DOTAWA_CONFIG=$(OTAWA_PATH)/bin/otawa-config ../../proc

clean:
	cd patmos && $(MAKE) clean
	cd build/otawa-patmos && $(MAKE) clean
	cd build/proc && $(MAKE) clean

install:
	cd build/otawa-patmos && $(MAKE) install
	cd build/proc && $(MAKE) install
