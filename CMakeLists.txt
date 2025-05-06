cmake_minimum_required(VERSION 3.28)
project(comp_club)

set(CMAKE_CXX_STANDARD 20)
find_package(Catch2 3 REQUIRED)

add_library(comp_lib lib/comp_club.cpp lib/comp_club.h)
add_executable(main main.cpp)
target_link_libraries(main comp_lib)

add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE comp_lib Catch2::Catch2WithMain)
