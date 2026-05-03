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
    WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing,
    BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing,
    PieceCNT
};
// clang-format on

constexpr Square operator++(Square &sq);
constexpr Side operator!(Side side);

namespace types {

constexpr Square square_of(File file, Rank rank);
constexpr File file_of(Square sq);
constexpr Rank rank_of(Square sq);
constexpr Side side_of(Piece piece);
constexpr Piece piece_of(Side us, PieceType type);
constexpr PieceType type_of(Piece piece);

} // namespace types

} // namespace cheslib

namespace cheslib { // definitions

constexpr Square operator++(Square &sq) {
    return sq = Square(sq + 1U);
}

constexpr Side operator!(Side side) {
    return Side(!(bool)side);
}

namespace types {

constexpr Square square_of(File file, Rank rank) {
    assert(file < FileCNT);
    assert(rank < RankCNT);
    return Square(rank << 3 | file); // rank * FileCNT + file
}

constexpr File file_of(Square sq) {
    assert(sq < SquareCNT);
    return File(sq & 7); // sq % 8
}

constexpr Rank rank_of(Square sq) {
    assert(sq < SquareCNT);
    return Rank(sq >> 3); // sq / 8
}

constexpr Side side_of(Piece piece) {
    assert(piece < PieceCNT);
    return Side(piece >= BlackPawn);
}

constexpr Piece piece_of(Side us, PieceType type) {
    assert(type < PieceTypeCNT);
    unsigned offset = (us == White) ? 0 : PieceTypeCNT;
    return Piece(type + offset);
}

constexpr PieceType type_of(Piece piece) {
    assert(piece < PieceCNT);
    unsigned offset = (piece < BlackPawn) ? 0 : BlackPawn;
    return PieceType(piece - offset);
}

} // namespace types

} // namespace cheslib
