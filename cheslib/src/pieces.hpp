#pragma once

#include <array>

#include "types.hpp"

namespace cheslib {

/**
 * see: https://www.chessprogramming.org/Bitboard_Board-Definition
 */
class Pieces {
  public:
    Pieces(std::array<Piece, SquareCNT> &&board);

    const std::array<Piece, SquareCNT> &board() const;
    Piece at(Square sq) const;

    Bitboard all() const;
    Bitboard all_of(Side us) const;
    Bitboard get(Piece piece) const;

    void put(Square sq, Piece piece);
    void move(Square from, Square to);
    Piece remove(Square sq);

  private:
    std::array<Piece, SquareCNT> _board;
    Bitboard _bitboards[PieceCNT];
    Bitboard _side[2];
    Bitboard _all;
};

} // namespace cheslib
