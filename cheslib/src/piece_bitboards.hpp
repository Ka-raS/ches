#pragma once

#include <bit>
#include <cassert>

#include "cheslib/types.hpp"
#include "utils.hpp"

namespace cheslib {

/**
 * TODO: write comments here
 * see: https://www.chessprogramming.org/Bitboard_Board-Definition
 */
class PieceBitboards {
  public:
    constexpr PieceBitboards(
        Bitboard wpawns, Bitboard wknights, Bitboard wbishops, Bitboard wrooks, Bitboard wqueens, Bitboard wking,
        Bitboard bpawns, Bitboard bknights, Bitboard bbishops, Bitboard brooks, Bitboard bqueens, Bitboard bking
    )
        : _pieces{wpawns, wknights, wbishops, wrooks, wqueens, wking,
                  bpawns, bknights, bbishops, brooks, bqueens, bking},
          _white(wpawns | wknights | wbishops | wrooks | wqueens | wking),
          _black(bpawns | bknights | bbishops | brooks | bqueens | bking), _all(_white | _black) {
        assert(std::popcount(wking) == 1);
        assert(std::popcount(bking) == 1);
    }

    static constexpr PieceBitboards initial() {
        return PieceBitboards(
            // clang-format off
            make_bitboard(SQUARE_A2, SQUARE_B2, SQUARE_C2, SQUARE_D2, SQUARE_E2, SQUARE_F2, SQUARE_G2, SQUARE_H2),
            make_bitboard(SQUARE_B1, SQUARE_G1),
            make_bitboard(SQUARE_C1, SQUARE_F1),
            make_bitboard(SQUARE_A1, SQUARE_H1),
            make_bitboard(SQUARE_D1),
            make_bitboard(SQUARE_E1),

            make_bitboard(SQUARE_A7, SQUARE_B7, SQUARE_C7, SQUARE_D7, SQUARE_E7, SQUARE_F7, SQUARE_G7, SQUARE_H7),
            make_bitboard(SQUARE_B8, SQUARE_G8),
            make_bitboard(SQUARE_C8, SQUARE_F8),
            make_bitboard(SQUARE_A8, SQUARE_H8),
            make_bitboard(SQUARE_D8),
            make_bitboard(SQUARE_E8)
            // clang-format on
        );
    }

    template <bool IsBlack>
    constexpr Bitboard get(Piece piece) const {
        assert(piece < PIECE_CNT);
        constexpr size_t offset = IsBlack ? PIECE_CNT : 0;
        return _pieces[offset + piece];
    }

    constexpr Bitboard white() const {
        return _white;
    }

    constexpr Bitboard black() const {
        return _black;
    }

    constexpr Bitboard all() const {
        return _all;
    }

  private:
    Bitboard _pieces[12];
    Bitboard _white, _black, _all;
};

} // namespace cheslib
