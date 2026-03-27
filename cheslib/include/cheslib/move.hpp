#pragma once

#include <cstdint>

#include "cheslib/constants.hpp"

namespace cheslib {

/**
 * See: https://www.chessprogramming.org/Encoding_Moves
 *
 * Compact chess move representation encoded into a `uint16_t`.
 * Bits 0-5: the "from" square
 * Bits 6-11: the "to" square
 * Bits 12-15: flags
 */
struct Move {
    int from;
    int to;
    Piece promotion;
};

} // namespace cheslib
