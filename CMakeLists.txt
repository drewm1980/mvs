cmake_minimum_required(VERSION 2.8.12.2)
project(libmvs)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
		"Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
		FORCE)
	set (CMAKE_C_FLAGS "-std=c99 -march=native -ffast-math -fopenmp")
	set (CMAKE_CXX_FLAGS "-std=c++11 -march=native -ffast-math -fopenmp")
endif(NOT CMAKE_BUILD_TYPE)

add_library(mvs SHARED mvs.c)
target_compile_options(mvs PRIVATE -std=c99 -O3 -Werror -Wall -march=native -ffast-math)
# Run these commands after the CMake build to try out clang:
# clang-4.0 -Rpass-analysis=loop-vectorize -fPIC -g -O3 -std=c99 -march=native -ffast-math -c mvs.c
target_link_libraries(mvs -lm)

# llvm-objdump -d mvs

