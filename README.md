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

patmos/         Gliss2 definition of the Patmos ISA
otawa-patmos/   OTAWA ISA description for Patmos
proc/		WCET analysis plugin
test/		Binary test files


Installation
------------

# Get otawa


# Create a Makefile.cfg, edit to your paths
make config
make
make install


Useful Commands/Quick guide
---------------------------

- patmos/disasm/disasm <elf>
  Disassemble an ELF file

- otawa-core/bin/dumpcfg -Did <elf> > out.dot
  Dump the CFG

- otawa-core/bin/owcet -s patmos.osx <elf>
  Run the WCET analysis

