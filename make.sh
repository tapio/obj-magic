#!/bin/sh

EXE="wavefront-modify"
CFLAGS="-O2"
clang++ $CFLAGS *.cpp -o $EXE
