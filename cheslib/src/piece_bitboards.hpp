#pragma once

#include <bit>
#include <cassert>

#include "cheslib/types.hpp"
#include "utils.hpp"

namespace ches {

/**
 * TODO: write comments here
 * see: https://www.chessprogramming.org/Bitboard_Board-Definition
 */
class PieceBitboards {
  public:
    constexpr PieceBitboards() : _pieces{0}, _side{0}, _all(0) {
    }

    constexpr PieceBitboards(
        Bitboard wpawn, Bitboard wknight, Bitboard wbishop, Bitboard wrook, Bitboard wqueen, Bitboard wking,
        Bitboard bpawn, Bitboard bknight, Bitboard bbishop, Bitboard brook, Bitboard bqueen, Bitboard bking
    )
        : _pieces{wpawn, wknight, wbishop, wrook, wqueen, wking, bpawn, bknight, bbishop, brook, bqueen, bking},
          _side{wpawn | wknight | wbishop | wrook | wqueen | wking, bpawn | bknight | bbishop | brook | bqueen | bking},
          _all(_side[0] | _side[1]) {
        assert(std::popcount(wking) == 1);
        assert(std::popcount(bking) == 1);
    }

    static constexpr PieceBitboards initial() {
        return PieceBitboards(
            // clang-format off
            to_bitboard(SquareA2, SquareB2, SquareC2, SquareD2, SquareE2, SquareF2, SquareG2, SquareH2),
            to_bitboard(SquareB1, SquareG1),
            to_bitboard(SquareC1, SquareF1),
            to_bitboard(SquareA1, SquareH1),
            to_bitboard(SquareD1),
            to_bitboard(SquareE1),

            to_bitboard(SquareA7, SquareB7, SquareC7, SquareD7, SquareE7, SquareF7, SquareG7, SquareH7),
            to_bitboard(SquareB8, SquareG8),
            to_bitboard(SquareC8, SquareF8),
            to_bitboard(SquareA8, SquareH8),
            to_bitboard(SquareD8),
            to_bitboard(SquareE8)
            // clang-format on
        );
    }

    constexpr Bitboard all() const {
        return _all;
    }

    template <Side Us>
    constexpr Bitboard all() const {
        return _side[Us];
    }

    template <Side Us>
    constexpr Bitboard get(PieceType type) const {
        Piece piece = piece_of<Us>(type);
        return _pieces[piece];
    }

    constexpr Bitboard get(Piece piece) const {
        return _pieces[piece];
    }

    constexpr void set(Square sq, Piece piece) {
        assert(sq < SquareCNT);

        set_square(_all, sq);
        set_square(_pieces[piece], sq);
        set_square(_side[side_of(piece)], sq);
    }

    constexpr void unset(Square sq, Piece piece) {
        assert(sq < SquareCNT);

        clear_square(_all, sq);
        clear_square(_pieces[piece], sq);
        clear_square(_side[side_of(piece)], sq);
    }

  private:
    Bitboard _pieces[PieceCNT];
    Bitboard _side[2];
    Bitboard _all;
};

} // namespace ches
