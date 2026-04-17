add_library(cheslib STATIC
    src/attacks.cpp
    src/movegen.cpp
    src/position.cpp
    src/zobrist.cpp
)

target_include_directories(cheslib PUBLIC include/)
target_compile_features(cheslib PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(cheslib PRIVATE /constexpr:steps100000000)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(cheslib PRIVATE -fconstexpr-ops-limit=100000000)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(cheslib PRIVATE -fconstexpr-steps=100000000)
endif()

if(CMAKE_TESTING_ENABLED)
    add_subdirectory(tests)
endif()

if(CHESLIB_BUILD_TOOLS)
    add_subdirectory(tools)
endif()