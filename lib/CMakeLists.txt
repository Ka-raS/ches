cmake_minimum_required(VERSION 3.14)
project(cheslib VERSION 0.1.0 LANGUAGES C CXX)

add_library(${PROJECT_NAME} STATIC
    src/bitboard.cpp
    src/board.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PUBLIC raylib)

target_include_directories(${PROJECT_NAME} 
    PUBLIC include/
    PRIVATE src/
)