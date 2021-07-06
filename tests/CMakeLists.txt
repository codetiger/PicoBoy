cmake_minimum_required(VERSION 3.0.0)

project (picoboytest)

set(CMAKE_CXX_FLAGS_DEBUG "-O3 -g -std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -std=c++14")
set(CMAKE_BUILD_TYPE Debug)

include_directories(. gboy/)
add_executable(picoboytest 
    test.cpp 
    ../gboy/Cartridge.cc
    ../gboy/MMU.cc
    ../gboy/CPU.cc)
