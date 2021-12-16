cmake_minimum_required(VERSION 3.16)
project(redis_parity C)

set(CMAKE_C_STANDARD 99)

add_library(redis_parity SHARED library.c library.h src/assist/parity.h src/assist/parity.c src/assist/bitOp.c src/assist/bitOp.h src/filter.c src/assist/parityHash.c src/assist/parityHash.h src/assist/base.h src/assist/queue.h src/assist/queue.c src/sliceAndPfOp.c src/sliceAndPfOp.h src/filterInit.c src/filterInit.h src/assist/bash.c src/persist/persist.c src/persist/persist.h src/keyAddAndDelOp.c src/keyAddAndDelOp.h)