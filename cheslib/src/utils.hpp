#pragma once

#include <bit>
#include <cassert>
#include <cstdlib>

#include "cheslib/types.hpp"

namespace cheslib {

constexpr bool has_square(Bitboard bb, Square sq) {
    assert(sq < SQUARE_CNT);
    return bb & (1ULL << sq);
}

constexpr void set_square(Bitboard &bb, Square sq) {
    assert(sq < SQUARE_CNT);
    bb |= (1ULL << sq);
}

constexpr File file_of(Square sq) {
    assert(sq < SQUARE_CNT);
    return File(sq & 7); // sq % 8
}

constexpr Rank rank_of(Square sq) {
    assert(sq < SQUARE_CNT);
    return Rank(sq >> 3); // sq / 8
}

constexpr Square pop_lsb(Bitboard &bb) {
    Square sq = Square(std::countr_zero(bb));
    bb &= (bb - 1);
    return sq;
}

constexpr Square make_square(File file, Rank rank) {
    assert(file < FILE_CNT);
    assert(rank < RANK_CNT);
    return Square(rank << 3 | file); // rank * 8 + file
}

constexpr Square shift_square(Square from, int8_t step) {
    assert(from < SQUARE_CNT);

    // check rank wraparound
    Square to = Square(from + step);
    if (to >= SQUARE_CNT) {
        return SQUARE_CNT;
    }

    // check file wraparound
    File from_file = file_of(from);
    File to_file = file_of(to);
    if (std::abs(to_file - from_file) > 2) {
        return SQUARE_CNT;
    }

    return to;
}

template <typename... Args>
    requires(std::is_same_v<Args, Square> && ...)
constexpr Bitboard make_bitboard(Args... squares) {
    Bitboard bb = 0;
    (set_square(bb, squares), ...);
    return bb;
}

constexpr Bitboard rank_bitboard(Rank rank) {
    assert(rank < RANK_CNT);
    constexpr Bitboard rank_1 =
        make_bitboard(SQUARE_A1, SQUARE_B1, SQUARE_C1, SQUARE_D1, SQUARE_E1, SQUARE_F1, SQUARE_G1, SQUARE_H1);

    int padding_squares = rank << 3; // rank * 8
    return rank_1 << padding_squares;
}

constexpr Bitboard file_bitboard(File file) {
    assert(file < FILE_CNT);
    constexpr Bitboard file_a =
        make_bitboard(SQUARE_A1, SQUARE_A2, SQUARE_A3, SQUARE_A4, SQUARE_A5, SQUARE_A6, SQUARE_A7, SQUARE_A8);

    return file_a << file;
}

} // namespace cheslib
