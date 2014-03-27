Patmos Plugin for OTAWA
=======================

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


Useful Commands
---------------
- patmos/disasm/disasm <elf>
  Disassemble an ELF file

- otawa-core/bin/dumpcfg -Did <elf> > out.dot
  Dump the CFG

- otawa-core/bin/owcet -s patmos.osx <elf>
  Run the WCET analysis
