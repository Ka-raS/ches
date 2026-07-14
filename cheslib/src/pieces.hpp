#pragma once

#include "types.hpp"

#include <array>

namespace cheslib {

/**
 * see: https://www.chessprogramming.org/Bitboard_Board-Definition
 */
class Pieces {
  public:
    explicit Pieces(const std::array<Piece, SquareCNT> &board);
    static std::array<Piece, SquareCNT> initial();

    const std::array<Piece, SquareCNT> &board() const;
    Piece at(Square square) const;
    int count(Piece piece) const;
    Square king_of(Side us) const;

    Bitboard all() const;
    Bitboard all_of(Side us) const;
    Bitboard get(Piece piece) const;

    void put(Piece piece, Square at);
    void move(Square from, Square to);
    Piece remove(Square at);

  private:
    std::array<Piece, SquareCNT> _board;
    Bitboard _bitboards[PieceCNT];
    Bitboard _side[2];
    Bitboard _all;
};

} // namespace cheslib
