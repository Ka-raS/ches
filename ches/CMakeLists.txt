# raylib dependency

find_package(raylib 5.5 QUIET)

if(NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
        URL_HASH SHA256=aea98ecf5bc5c5e0b789a76de0083a21a70457050ea4cc2aec7566935f5e258e
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
    )

    FetchContent_MakeAvailable(raylib)
endif()

# ches project

add_executable(ches
    src/main.cpp
    src/game.cpp
    src/window.cpp
)

target_link_libraries(ches
    PRIVATE raylib
    PRIVATE cheslib
)

target_compile_features(ches PRIVATE cxx_std_20)
