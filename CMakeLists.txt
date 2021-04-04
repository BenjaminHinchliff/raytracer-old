cmake_minimum_required(VERSION 3.12.0)
project(raytracer VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)

option(RAY_ENABLE_TESTS "whether to enable testing via ctest" ON)

add_subdirectory("src")

if(RAY_ENABLE_TESTS)
  enable_testing()
  add_subdirectory("tests")
endif()
