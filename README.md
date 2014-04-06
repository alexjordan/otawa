Patmos Plugin for OTAWA
=======================

This repository contains the Patmos plugin code for OTAWA.

Find out more about OTAWA at

  http://www.otawa.fr/

Find out more about Patmos / T-CREST at

  http://patmos.compute.dtu.dk/
  http://www.t-crest.org/


Contained Files
---------------

```
patmos/         Gliss2 definition of the Patmos ISA
otawa-patmos/   Extends the Patmos ISA with semantic information for OTAWA
patmos-wcet/	OTAWA WCET analysis plugin and script
test/		Binary test files
```

Installation
------------

# Get otawa

```
make checkout
```

# Create a Makefile.cfg, edit to your paths

```
make config
make
make install
```


Useful Commands/Quick guide
---------------------------

- patmos/disasm/disasm <elf>
  Disassemble an ELF file

- otawa-core/bin/dumpcfg -Did <elf> > out.dot
  Dump the CFG

- otawa-core/bin/odfa test/bs.elf -c -s -C -r "\$r31=0x400000" -r "\$s6=0x300000" -r "\$r30=0x400000"
  Run a dataflow analysis

- otawa-core/bin/owcet -s patmos.osx <elf>
  Run the WCET analysis

Acknowledgements
----------------

This work was partially funded under the European Union's 7th Framework
Programme under grant agreement no. 288008: Time-predictable Multi-Core
Architecture for Embedded Systems (T-CREST) and under the COST Short Term 
Scientific Mission no. COST-STSM-IC1202-17297.

