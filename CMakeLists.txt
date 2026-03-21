cmake_minimum_required(VERSION 3.14)
project(ches VERSION 0.1.0 LANGUAGES C CXX)

# raylib dependency

set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET)

if(NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        TLS_VERIFY OFF # if cannot verify SSL certificate
    )

    set(FETCHCONTENT_QUIET OFF)
    FetchContent_MakeAvailable(raylib)
endif()

# cheslib dependency

add_subdirectory(lib/cheslib/)

# ches project

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/game.cpp
    src/window.cpp
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE raylib
    PRIVATE cheslib
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
