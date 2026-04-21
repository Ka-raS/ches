# Catch2 dependency

find_package(Catch2 3.13.0 QUIET)

if(NOT Catch2_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        Catch2  
        URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.13.0.tar.gz
        URL_HASH SHA256=650795f6501af514f806e78c554729847b98db6935e69076f36bb03ed2e985ef
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
    )

    FetchContent_MakeAvailable(Catch2)
endif()

# cheslib tests

add_executable(cheslib_test
    test_attacks.cpp
    test_move.cpp
    test_movegen.cpp
    test_pieces.cpp
    test_position.cpp
    test_state.cpp
    test_utils.cpp
    test_zobrist.cpp
)

target_include_directories(cheslib_test PRIVATE ../src)
target_link_libraries(cheslib_test PRIVATE cheslib Catch2::Catch2WithMain)
target_compile_features(cheslib_test PRIVATE cxx_std_20)

include(Catch)
catch_discover_tests(cheslib_test)
add_custom_target(run_cheslib_tests ALL
    COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS cheslib_test
)