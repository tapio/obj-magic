#!/bin/sh

EXENAME="obj-magic"
CFLAGS="-O2 -Wall -Wextra -Wno-unused-parameter"

if [ "x$CXX" = "x" ]; then
	# Default to GCC
	CXX=g++
	# Prefer clang++ if available
	which clang++ > /dev/null
	if [ $? -eq 0 ]; then
		CXX=clang++
	fi
fi

set -x
$CXX $CFLAGS src/*.cpp -o $EXENAME

