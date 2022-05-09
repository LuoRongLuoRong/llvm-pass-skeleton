#!/bin/bash

export CC=wllvm
export LLVM_COMPILER=clang
export CFLAGS="-g -O0"

make server
