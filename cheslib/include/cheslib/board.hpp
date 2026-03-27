#pragma once

#include <cstdint>

#include "cheslib/constants.hpp"

namespace cheslib {

// clang-format off
/**
 * See: https://www.chessprogramming.org/Bitboards
 *
 * Bitboard representation of a chessboard using `uint64_t`.
 * Each `uint64_t` with its 64 bits represent 64 squares on chessboard
 * If bit=1 then a piece of that type is on the square
 */
struct Board {
    uint64_t whitePawns   = (1ULL << A2) | (1ULL << B2) | (1ULL << C2) | (1ULL << D2) |
                            (1ULL << E2) | (1ULL << F2) | (1ULL << G2) | (1ULL << H2);
    uint64_t whiteKnights = (1ULL << B1) | (1ULL << G1);
    uint64_t whiteBishops = (1ULL << C1) | (1ULL << F1);
    uint64_t whiteRooks   = (1ULL << A1) | (1ULL << H1);
    uint64_t whiteQueen   = (1ULL << D1);
    uint64_t whiteKing    = (1ULL << E1);

    uint64_t blackPawns   = (1ULL << A7) | (1ULL << B7) | (1ULL << C7) | (1ULL << D7) |
                            (1ULL << E7) | (1ULL << F7) | (1ULL << G7) | (1ULL << H7);
    uint64_t blackKnights = (1ULL << B8) | (1ULL << G8);
    uint64_t blackBishops = (1ULL << C8) | (1ULL << F8);
    uint64_t blackRooks   = (1ULL << A8) | (1ULL << H8);
    uint64_t blackQueen   = (1ULL << D8);
    uint64_t blackKing    = (1ULL << E8);

    uint64_t allWhitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueen | whiteKing;
    uint64_t allBlackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueen | blackKing;
    uint64_t allPieces = allWhitePieces | allBlackPieces;
};
// clang-format on

} // namespace cheslib
