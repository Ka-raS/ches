#pragma once

#include <cstdint>

namespace cheslib {

// clang-format off

enum Square : uint8_t {
    SquareA1, SquareB1, SquareC1, SquareD1, SquareE1, SquareF1, SquareG1, SquareH1,
    SquareA2, SquareB2, SquareC2, SquareD2, SquareE2, SquareF2, SquareG2, SquareH2,
    SquareA3, SquareB3, SquareC3, SquareD3, SquareE3, SquareF3, SquareG3, SquareH3,
    SquareA4, SquareB4, SquareC4, SquareD4, SquareE4, SquareF4, SquareG4, SquareH4,
    SquareA5, SquareB5, SquareC5, SquareD5, SquareE5, SquareF5, SquareG5, SquareH5,
    SquareA6, SquareB6, SquareC6, SquareD6, SquareE6, SquareF6, SquareG6, SquareH6,
    SquareA7, SquareB7, SquareC7, SquareD7, SquareE7, SquareF7, SquareG7, SquareH7,
    SquareA8, SquareB8, SquareC8, SquareD8, SquareE8, SquareF8, SquareG8, SquareH8,
    SquareCNT
};
// clang-format on

enum Rank : uint8_t {
    Rank1,
    Rank2,
    Rank3,
    Rank4,
    Rank5,
    Rank6,
    Rank7,
    Rank8,
    RankCNT
};

enum File : uint8_t {
    FileA,
    FileB,
    FileC,
    FileD,
    FileE,
    FileF,
    FileG,
    FileH,
    FileCNT
};

enum PieceType : uint8_t {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
    PieceTypeCNT
};

enum Side : bool {
    White,
    Black
};

enum Piece : uint8_t {
    WhitePawn,
    WhiteKnight,
    WhiteBishop,
    WhiteRook,
    WhiteQueen,
    WhiteKing,

    BlackPawn,
    BlackKnight,
    BlackBishop,
    BlackRook,
    BlackQueen,
    BlackKing,

    PieceCNT
};

constexpr Square operator++(Square &sq) {
    return sq = Square(sq + 1U);
}

constexpr Rank operator++(Rank &r) {
    return r = Rank(r + 1U);
}

constexpr File operator++(File &f) {
    return f = File(f + 1U);
}

constexpr PieceType operator++(PieceType &p) {
    return p = PieceType(p + 1U);
}

constexpr Piece operator++(Piece &p) {
    return p = Piece(p + 1U);
}

} // namespace cheslib
