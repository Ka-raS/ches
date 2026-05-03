#pragma once

#include <bit>

#include "cheslib/types.hpp"

namespace cheslib {

/**
 * `uint64_t` represents 64 squares on chessboard
 * if bit=1 then a piece type is on the square
 * see: https://www.chessprogramming.org/Bitboards
 */
using Bitboard = uint64_t;

enum Direction : int8_t {
    North = SquareA2 - SquareA1,
    East = SquareB1 - SquareA1,
    South = -North,
    West = -East,

    NorthEast = North + East,
    SouthEast = South + East,
    SouthWest = -NorthEast,
    NorthWest = -SouthEast
};

constexpr Rank operator++(Rank &r);
constexpr File operator++(File &f);
constexpr PieceType operator++(PieceType &p);
constexpr Piece operator++(Piece &p);

namespace types {

constexpr Square square_behind(Side us, Square sq);
constexpr Square pop_lsb(Bitboard &bb);

constexpr Bitboard bitboard_of(Rank rank);
constexpr Bitboard bitboard_of(File file);
constexpr Bitboard bitboard_of(std::same_as<Square> auto... squares);

constexpr bool has_square(Bitboard bb, Square square);
constexpr void set_square(Bitboard &bb, Square square);
constexpr void unset_square(Bitboard &bb, Square square);

} // namespace types

} // namespace cheslib

namespace cheslib { // definitions

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

namespace types {

constexpr Square square_behind(Side us, Square sq) {
    if (us == White) {
        assert(sq >= SquareA2);
    } else {
        assert(sq <= SquareH7);
    }

    Direction backward = (us == White) ? South : North;
    return Square(sq + (int)backward);
}

constexpr Square pop_lsb(Bitboard &bb) {
    assert(bb != 0);
    Square sq = Square(std::countr_zero(bb));
    bb &= (bb - 1);
    return sq;
}

constexpr Bitboard bitboard_of(std::same_as<Square> auto... squares) {
    (assert(squares < SquareCNT), ...);
    return ((1ULL << squares) | ...);
}

constexpr Bitboard bitboard_of(Rank rank) {
    assert(rank < RankCNT);
    constexpr Bitboard rank_1 =
        bitboard_of(SquareA1, SquareB1, SquareC1, SquareD1, SquareE1, SquareF1, SquareG1, SquareH1);

    unsigned padding_squares = rank << 3; // rank * FileCNT
    return rank_1 << padding_squares;
}

constexpr Bitboard bitboard_of(File file) {
    assert(file < FileCNT);
    constexpr Bitboard file_a =
        bitboard_of(SquareA1, SquareA2, SquareA3, SquareA4, SquareA5, SquareA6, SquareA7, SquareA8);
    return file_a << file;
}

constexpr bool has_square(Bitboard bb, Square square) {
    assert(square < SquareCNT);
    return (bb >> square) & 1;
}

constexpr void set_square(Bitboard &bb, Square square) {
    bb |= bitboard_of(square);
}

constexpr void unset_square(Bitboard &bb, Square square) {
    bb &= ~bitboard_of(square);
}

} // namespace types

} // namespace cheslib
