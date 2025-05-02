cmake_minimum_required(VERSION 3.28)
project(comp_club)

set(CMAKE_CXX_STANDARD 20)

add_library(comp_lib lib/comp_club.cpp lib/comp_club.h)

add_executable(main main.cpp)

target_link_libraries(main comp_lib)
