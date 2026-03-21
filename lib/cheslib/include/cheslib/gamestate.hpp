#pragma once
#include "cheslib/square.hpp"

namespace cheslib {

struct GameState {
    bool whiteToMove = true;

    bool canWhiteShortCastle = true;
    bool canWhiteLongCastle  = true;
    bool canBlackShortCastle = true;
    bool canBlackLongCastle  = true;

    sq::Square enPassant = sq::NO_SQUARE; // where en passant is possible 
    int halfmoveCounter  = 0;             // for counting 50 move no capture = draw
    int fullmoveCounter  = 1;
};

} // namespace cheslib
