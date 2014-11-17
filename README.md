# OTAWA Patmos #

This repository combines OTAWA [1] and its framework dependencies with the WCET
plugin for the Patmos architecture [2].

A [build script](misc/build.sh) helps with building the integrated tool during development.

Changes in OTAWA are planned to be merged manually from their repositories as
required.

[1] http://otawa.fr/  
[2] https://github.com/t-crest/patmos-otawa  


## Dependencies ##

After cloning this repository run

    $ git submodule update --init

to fetch the Patmos plugin which is retained as a submodule.

Also `lp_solve` needs to be installed on your system for OTAWA's IPET solver.


## Build Instructions ##

Make sure that the paths at the top of [build.sh](misc/build.sh) are set up
correctly for your system.

To build and install everything run

    $ misc/build.sh

The default install destination is `<checkout-dir>/local`.
If, for example, you want to selectively rebuild (and install) the Patmos
plugin after some changes in OTAWA itself, run

    $ misc/build.sh otawa patmos

To see a limited set of further build targets and options

    $ misc/build.sh --help
