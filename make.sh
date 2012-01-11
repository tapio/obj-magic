#!/bin/sh

EXENAME="obj-magic"
CXX=clang++
CFLAGS="-O2 -Wall -Wextra -Wno-unused-parameter"
$CXX $CFLAGS *.cpp -o $EXENAME
