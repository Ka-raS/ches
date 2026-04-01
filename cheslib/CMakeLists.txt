cmake_minimum_required(VERSION 3.14)
project(cheslib VERSION 0.1.0 LANGUAGES C CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    enable_testing()
endif()

add_library(${PROJECT_NAME} STATIC
    src/attack_tables.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /constexpr:steps=150000000)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PUBLIC -fconstexpr-steps=150000000)
else() # GCC
    target_compile_options(${PROJECT_NAME} PUBLIC -fconstexpr-ops-limit=150000000)
endif()

add_subdirectory(tests)