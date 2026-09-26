cmake_minimum_required(VERSION 3.23)
project(ches VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

include(CTest)

add_subdirectory(ches)
add_subdirectory(cheslib)
