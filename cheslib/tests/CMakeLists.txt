# Catch2 tests

set(Catch2_VERSION 3.13.0)
find_package(Catch2 ${Catch2_VERSION} QUIET)

if(NOT Catch2_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        Catch2  
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/catchorg/Catch2/archive/refs/tags/v${Catch2_VERSION}.tar.gz
        # TLS_VERIFY OFF # if cannot verify SSL certificate
    )

    set(FETCHCONTENT_QUIET OFF)
    FetchContent_MakeAvailable(Catch2)
endif()

set(NAME cheslib_test)

add_executable(${NAME}
    test_attack_tables.cpp 
)

target_include_directories(${NAME} PRIVATE ../src)

target_link_libraries(${NAME} 
    PRIVATE cheslib Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(${NAME})