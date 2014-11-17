#!/bin/bash

if [ ! -d gliss2 ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/gliss2/trunk gliss2; fi
if [ ! -d elm ];   then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/elm/trunk     elm; fi
if [ ! -d gel ];   then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/gel/trunk     gel; fi
if [ ! -d otawa ]; then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/otawa/trunk   otawa; fi

if [ ! -d ppc2 ];    then hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/ppc2/trunk    ppc2; fi
if [ ! -d patmos ];  then git submodule add https://github.com/alexjordan/patmos-otawa.git patmos; fi
