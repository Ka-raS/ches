add_library(cheslib STATIC)

target_compile_features(cheslib PUBLIC cxx_std_20)

target_include_directories(cheslib 
    PUBLIC include/
    PRIVATE src/
)

target_sources(cheslib PRIVATE
    src/attacks.cpp
    src/engine.cpp
    src/evaluate.cpp
    src/history_heuristic.cpp
    src/movegen.cpp
    src/negamax.cpp
    src/pieces.cpp
    src/position_state.cpp
    src/position.cpp
    src/thread.cpp
    src/transposition.cpp
    src/zobrist.cpp
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(cheslib PRIVATE "/constexpr:steps200000000")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(cheslib PRIVATE "/clang:-fconstexpr-steps=200000000")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang|IntelLLVM")
    target_compile_options(cheslib PRIVATE "-fconstexpr-steps=200000000")
else() # Fallback to GCC
    target_compile_options(cheslib PRIVATE "-fconstexpr-ops-limit=200000000")
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

add_subdirectory(tools)