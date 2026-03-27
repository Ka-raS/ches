cmake_minimum_required(VERSION 3.14)
project(cheslib VERSION 0.1.0 LANGUAGES C CXX)

add_library(${PROJECT_NAME} INTERFACE)
# add_library(${PROJECT_NAME} STATIC
#     # src/bitboard.cpp
#     # src/board.cpp
#     # src/move.cpp
# )


# target_include_directories(${PROJECT_NAME} 
#     PUBLIC include/
#     PRIVATE src/
# )
target_include_directories(${PROJECT_NAME} 
    INTERFACE include/
)

# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

if(MSVC)
    target_compile_options(${PROJECT_NAME} INTERFACE /constexpr:steps=150000000)
else()
    target_compile_options(${PROJECT_NAME} INTERFACE -fconstexpr-ops-limit=150000000)
endif()