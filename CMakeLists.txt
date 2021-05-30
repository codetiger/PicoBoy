cmake_minimum_required(VERSION 3.0.0)

project (picoboy)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")
set(CMAKE_BUILD_TYPE Debug)

find_package(SDL2 REQUIRED)
include_directories(SDL2Test ${SDL2_INCLUDE_DIRS})

include_directories(. gboy/)
add_executable(picoboy 
    picoboy.cpp 
    gboy/GBoy.cc 
    gboy/Cartridge.cc 
    gboy/CPU.cc 
    gboy/MMU.cc 
    gboy/PPU.cc 
    gboy/Timer.cc)
target_link_libraries(picoboy ${SDL2_LIBRARIES})
