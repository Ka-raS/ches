#pragma once
#include <cstdint>
#include "cheslib/pieces.hpp"

namespace cheslib {

/**
 * 64bit uints represent 64 squares chessboard
 * bit=1: a piece of that type is on the square
 */
struct BitBoard {
    uint64_t whitePawns;
    uint64_t whiteKnights;
    uint64_t whiteBishops;
    uint64_t whiteRooks;
    uint64_t whiteQueen;
    uint64_t whiteKing;
    
    uint64_t blackPawns;
    uint64_t blackKnights;
    uint64_t blackBishops;
    uint64_t blackRooks;
    uint64_t blackQueen;
    uint64_t blackKing;

    BitBoard();
    Piece pieceAt(size_t row, size_t col) const;
};

// the numbers mason what do they mean????

} // namespace cheslib
