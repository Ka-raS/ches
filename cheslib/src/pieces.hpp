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
    int count(Piece piece) const;
    Square king_of(Side us) const;

    Bitboard all() const;
    Bitboard all_of(Side us) const;
    Bitboard get(Piece piece) const;
    Bitboard get(Side us, PieceType type) const;

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
