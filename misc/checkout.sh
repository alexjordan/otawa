#!/bin/bash

# Path containing the otawa related hg repositories
ROOT_DIR=$(pwd)


if [ ! -d gliss2 ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/gliss2/trunk $ROOT_DIR/gliss2; fi
if [ ! -d elm ];   then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/elm/trunk     $ROOT_DIR/elm; fi
if [ ! -d gel ];   then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/gel/trunk     $ROOT_DIR/gel; fi
if [ ! -d otawa ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/otawa/trunk   $ROOT_DIR/otawa; fi

if [ ! -d ppc2 ];         then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/ppc2/trunk        $ROOT_DIR/ppc2; fi
#if [ ! -d patmos-otawa ]; then git clone https://github.com/t-crest/patmos-otawa.git $ROOT_DIR/patmos; fi
