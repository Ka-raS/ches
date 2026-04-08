cmake_minimum_required(VERSION 3.14)
project(cheslib VERSION 0.1.0 LANGUAGES C CXX)

add_library(${PROJECT_NAME} STATIC
    src/attack_tables.cpp
    src/move_generation.cpp
    src/position.cpp
    src/zobrist.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /constexpr:steps=300000000)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PUBLIC -fconstexpr-steps=300000000)
else() # GCC
    target_compile_options(${PROJECT_NAME} PUBLIC -fconstexpr-ops-limit=300000000)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    enable_testing()
endif()

if(CMAKE_TESTING_ENABLED)
    add_subdirectory(tests)
endif()

option(CHESLIB_BUILD_TOOLS "Build cheslib helper tools" OFF)

if(CHESLIB_BUILD_TOOLS)
    add_subdirectory(tools)
endif()