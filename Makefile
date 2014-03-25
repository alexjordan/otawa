
OTAWA_PATH=$(PWD)/../../otawa-core
GLISS_PATH=$(PWD)/../gliss2

config:
	cd build && cmake -DOTAWA_CONFIG=$(OTAWA_PATH)/bin/otawa-config -DGLISS_PATH=$(GLISS_PATH) ../otawa-patmos
