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
    static Pieces initial();

    const std::array<Piece, SquareCNT> &board() const;
    Piece at(Square sq) const;

    template <Side Us>
    Bitboard all() const;
    Bitboard all() const;

    template <Side Us>
    Bitboard get(PieceType type) const;

    template <Side Us>
    void put(Square sq, Piece piece);

    template <Side Us>
    void move(Square from, Square to);

    template <Side Us>
    Piece remove(Square sq);

  private:
    std::array<Piece, SquareCNT> _board;
    Bitboard _bitboards[PieceCNT];
    Bitboard _side[2];
    Bitboard _all;
};

} // namespace cheslib
