#!/usr/bin/env bash

clang-4.0 -c -O3 -std=c99 -march=native mvs.c -o mvs
llvm-objdump -d mvs
