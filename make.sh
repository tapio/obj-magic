#!/bin/sh

EXE="obj-magic"
CFLAGS="-O2"
clang++ $CFLAGS *.cpp -o $EXE
