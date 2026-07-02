#pragma once

#include <cassert>
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

enum Rank : uint8_t {
    Rank1, Rank2, Rank3, Rank4, Rank5, Rank6, Rank7, Rank8,
    RankCNT
};

enum File : uint8_t {
    FileA, FileB, FileC, FileD, FileE, FileF, FileG, FileH,
    FileCNT
};

enum PieceType : uint8_t {
    Pawn, Knight, Bishop, Rook, Queen, King,
    PieceTypeCNT
};

enum Side : bool {
    White, Black
};

enum Piece : uint8_t {
    WhitePawn, BlackPawn, WhiteKnight, BlackKnight, WhiteBishop, BlackBishop,
    WhiteRook, BlackRook, WhiteQueen, BlackQueen, WhiteKing, BlackKing,
    PieceCNT
};
// clang-format on

constexpr Square operator++(Square &square) {
    return square = Square(square + 1u);
}

constexpr Rank operator++(Rank &rank) {
    return rank = Rank(rank + 1u);
}

constexpr File operator++(File &file) {
    return file = File(file + 1u);
}

constexpr PieceType operator++(PieceType &type) {
    return type = PieceType(type + 1u);
}

constexpr Side operator!(Side side) {
    return Side{!bool{side}};
}

constexpr Piece operator++(Piece &piece) {
    return piece = Piece(piece + 1u);
}

constexpr Square square_of(const File file, const Rank rank) {
    assert(file < FileCNT);
    assert(rank < RankCNT);

    const Square square = Square(rank << 3 | file); // rank * FileCNT + file

    assert(square < SquareCNT);
    return square;
}

constexpr Rank rank_of(const Square square) {
    assert(square < SquareCNT);

    const Rank rank = Rank(square >> 3); // square / FileCNT

    assert(rank < RankCNT);
    return rank;
}

constexpr File file_of(const Square square) {
    assert(square < SquareCNT);

    const File file = File(square & 0b111); // square % FileCNT

    assert(file < FileCNT);
    return file;
}

constexpr PieceType type_of(const Piece piece) {
    assert(piece < PieceCNT);

    const PieceType type = PieceType(piece >> 1);

    assert(type < PieceTypeCNT);
    return type;
}

constexpr Side side_of(const Piece piece) {
    assert(piece < PieceCNT);
    return Side(piece & 1u);
}

constexpr Piece piece_of(const Side us, const PieceType type) {
    assert(type < PieceTypeCNT);

    const Piece piece = Piece(type << 1 | us); // lsb is Side

    assert(piece < PieceCNT);
    return piece;
}

constexpr Square flip_rank(const Square square) {
    assert(square < SquareCNT);

    const Square flipped = Square(square ^ 0b111'000);

    assert(flipped < SquareCNT);
    return flipped;
}

constexpr Square flip_file(const Square square) {
    assert(square < SquareCNT);

    const Square flipped = Square(square ^ 0b111);

    assert(flipped < SquareCNT);
    return flipped;
}

} // namespace cheslib
