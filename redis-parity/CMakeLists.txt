cmake_minimum_required(VERSION 3.16)
project(redis_parity C)

set(CMAKE_C_STANDARD 99)

add_library(redis_parity SHARED library.c library.h src/parity.h src/parity.c src/bitOp.c src/bitOp.h src/filter.c src/parityHash.c src/parityHash.h src/base.h src/queue.h src/queue.c)