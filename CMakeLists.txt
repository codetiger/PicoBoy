cmake_minimum_required(VERSION 3.0.0)

project (picoboy)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_BUILD_TYPE Release)

find_package(SDL2 REQUIRED)
include_directories(SDL2Test ${SDL2_INCLUDE_DIRS})

include_directories(. gboy/)
add_executable(picoboy picoboy.cpp)
target_link_libraries(picoboy ${SDL2_LIBRARIES})
