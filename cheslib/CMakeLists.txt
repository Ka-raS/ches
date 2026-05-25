add_library(cheslib STATIC)

target_include_directories(cheslib PUBLIC include/)
target_compile_features(cheslib PRIVATE cxx_std_20)

target_sources(cheslib PRIVATE
    src/attacks.cpp
    src/engine.cpp
    src/eval.cpp
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

target_compile_options(cheslib
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-mbmi2>
        $<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-fconstexpr-ops-limit=200000000>
        $<$<CXX_COMPILER_ID:Clang>:-fconstexpr-steps=200000000>
        $<$<CXX_COMPILER_ID:MSVC>:/constexpr:steps200000000>
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
