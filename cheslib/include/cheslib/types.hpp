#pragma once

#include <cassert>
#include <cstdint>

namespace cheslib {

enum Square : uint8_t {
    // clang-format off
    SquareA1, SquareB1, SquareC1, SquareD1, SquareE1, SquareF1, SquareG1, SquareH1,
    SquareA2, SquareB2, SquareC2, SquareD2, SquareE2, SquareF2, SquareG2, SquareH2,
    SquareA3, SquareB3, SquareC3, SquareD3, SquareE3, SquareF3, SquareG3, SquareH3,
    SquareA4, SquareB4, SquareC4, SquareD4, SquareE4, SquareF4, SquareG4, SquareH4,
    SquareA5, SquareB5, SquareC5, SquareD5, SquareE5, SquareF5, SquareG5, SquareH5,
    SquareA6, SquareB6, SquareC6, SquareD6, SquareE6, SquareF6, SquareG6, SquareH6,
    SquareA7, SquareB7, SquareC7, SquareD7, SquareE7, SquareF7, SquareG7, SquareH7,
    SquareA8, SquareB8, SquareC8, SquareD8, SquareE8, SquareF8, SquareG8, SquareH8,
    SquareCNT
    // clang-format on
};

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

enum MoveFlag : uint8_t {
    QuietMove = 0,
    DoublePawnPush = 0b001,
    ShortCastle = 0b010,
    LongCastle = 0b011,
    Capture = 0b100,
    EnPassant = 0b101,

    KnightPromo = 0b1000,
    BishopPromo = 0b1001,
    RookPromo = 0b1010,
    QueenPromo = 0b1011,

    KnightPromoCap = KnightPromo | Capture,
    BishopPromoCap = BishopPromo | Capture,
    RookPromoCap = RookPromo | Capture,
    QueenPromoCap = QueenPromo | Capture
};

constexpr Square operator++(Square &sq) {
    return sq = Square(sq + 1U);
}

namespace types {

constexpr File file_of(Square sq) {
    assert(sq < SquareCNT);
    return File(sq & 7); // sq % 8
}

constexpr Rank rank_of(Square sq) {
    assert(sq < SquareCNT);
    return Rank(sq >> 3); // sq / 8
}

} // namespace types

} // namespace cheslib
