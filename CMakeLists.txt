cmake_minimum_required(VERSION 3.18)
project(ches VERSION 0.1.0 LANGUAGES C CXX)

option(CHESLIB_BUILD_TOOLS "Build cheslib helper tools" OFF)

enable_testing()

add_subdirectory(ches)
add_subdirectory(cheslib)