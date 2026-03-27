#pragma once

#include "cheslib/constants.hpp"

namespace cheslib {

struct State {
    bool is_white_turn = true;

    bool can_white_short_castle = true;
    bool can_white_long_castle  = true;
    bool can_black_short_castle = true;
    bool can_black_long_castle  = true;

    // when a pawn moves two squares, set to the square behind the pawn
    int en_passant_target = NO_SQUARE;

    // count non pawn moves and non capture moves for 50-move rule to draw
    int halfmove_counter  = 0;
};

} // namespace cheslib
