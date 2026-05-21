#pragma once

#include <bit>
#include <cstddef>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

namespace cheslib {

/**
 * `uint64_t` represents 64 squares on chessboard
 * if bit=1 then a piece type is on the square
 * see: https://www.chessprogramming.org/Bitboards
 */
using Bitboard = uint64_t;

/**
 * Zobrist hash key
 * see: https://www.chessprogramming.org/Zobrist_Hashing
 */
using ZobristKey = uint64_t;

using Score = int16_t;

struct MoveScore {
    Move move;
    Score score;
};

/// negamax alpha beta pruning bound
enum class Bound : uint8_t {
    Exact,
    Lower,
    Upper
};

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

namespace types {

/**
 * @return the square behind `square` from `us` view,
 * example: square_behind(White, SquareE4) == SquareE3
 */
constexpr Square square_behind(Side us, Square square) {
    if (us == White) {
        assert(square >= SquareA2);
    } else {
        assert(square <= SquareH7);
    }

    Direction backward = (us == White) ? South : North;
    Square behind = Square(int(square) + backward);

    assert(behind < SquareCNT);
    return behind;
}

constexpr Square pop_lsb(Bitboard &bitboard) {
    assert(bitboard != 0);

    Square lsb = Square(std::countr_zero(bitboard));
    bitboard &= (bitboard - 1ull);

    assert(lsb < SquareCNT);
    return lsb;
}

constexpr Bitboard bitboard_of(std::same_as<Square> auto... squares) {
    assert(((squares < SquareCNT) && ...));
    return ((1ull << squares) | ...);
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
