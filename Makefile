
OTAWA_PATH=$(PWD)/../../otawa-core
GLISS_PATH=$(PWD)/../gliss2

.PHONY: all config clean install

all:
	cd patmos && $(MAKE)
	cd build && $(MAKE)

config:
	mkdir -p build
	cd build && cmake -DOTAWA_CONFIG=$(OTAWA_PATH)/bin/otawa-config -DGLISS_PATH=$(GLISS_PATH) ../otawa-patmos

clean:
	cd patmos && $(MAKE) clean
	cd build && $(MAKE) clean

install:
	cd build && $(MAKE) install
