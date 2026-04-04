#pragma once

#include <cstdint>

namespace cheslib {

/**
 * `uint64_t` represents 64 squares on chessboard
 * if bit=1 then a piece type is on the square
 * see: https://www.chessprogramming.org/Bitboards
 */
using Bitboard = uint64_t;

// clang-format off

enum Square : uint8_t {
    SQUARE_A1, SQUARE_B1, SQUARE_C1, SQUARE_D1, SQUARE_E1, SQUARE_F1, SQUARE_G1, SQUARE_H1,
    SQUARE_A2, SQUARE_B2, SQUARE_C2, SQUARE_D2, SQUARE_E2, SQUARE_F2, SQUARE_G2, SQUARE_H2,
    SQUARE_A3, SQUARE_B3, SQUARE_C3, SQUARE_D3, SQUARE_E3, SQUARE_F3, SQUARE_G3, SQUARE_H3,
    SQUARE_A4, SQUARE_B4, SQUARE_C4, SQUARE_D4, SQUARE_E4, SQUARE_F4, SQUARE_G4, SQUARE_H4,
    SQUARE_A5, SQUARE_B5, SQUARE_C5, SQUARE_D5, SQUARE_E5, SQUARE_F5, SQUARE_G5, SQUARE_H5,
    SQUARE_A6, SQUARE_B6, SQUARE_C6, SQUARE_D6, SQUARE_E6, SQUARE_F6, SQUARE_G6, SQUARE_H6,
    SQUARE_A7, SQUARE_B7, SQUARE_C7, SQUARE_D7, SQUARE_E7, SQUARE_F7, SQUARE_G7, SQUARE_H7,
    SQUARE_A8, SQUARE_B8, SQUARE_C8, SQUARE_D8, SQUARE_E8, SQUARE_F8, SQUARE_G8, SQUARE_H8,
    SQUARE_CNT
};
// clang-format on

enum Rank : uint8_t {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    RANK_CNT
};

enum File : uint8_t {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
    FILE_CNT
};

enum Piece : uint8_t {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    PIECE_CNT
};

enum Direction : int8_t {
    UP = SQUARE_A1 + SQUARE_A2,
    RIGHT = SQUARE_A1 + SQUARE_B1,
    DOWN = -UP,
    LEFT = -RIGHT,

    UP_RIGHT = UP + RIGHT,
    DOWN_RIGHT = DOWN + RIGHT,
    DOWN_LEFT = -UP_RIGHT,
    UP_LEFT = -DOWN_RIGHT
};

constexpr Square operator++(Square &sq) {
    return sq = Square(sq + 1U);
}

constexpr Piece operator++(Piece &p) {
    return p = Piece(p + 1U);
}

} // namespace cheslib
