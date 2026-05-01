#pragma once

#include "cheslib/types.hpp"

namespace cheslib {

/**
 * 16bit encoded move
 * bit layout:
 *  0-5   : from square
 *  6-11  : to square
 *  12-13 : special bits, if promotion move then it's promoted piece type
 *  14    : is capture
 *  15    : is promotion
 * see: https://www.chessprogramming.org/Encoding_Moves
 */
class Move {
  public:
    Move() = default;
    Move(Square from, Square to, MoveFlag flag);
    Square from() const;
    Square to() const;
    MoveFlag flag() const;
    PieceType promo_piece() const;
    bool is_promotion() const;
    bool is_capture() const;
    bool operator==(const Move &) const = default;

  private:
    uint16_t _data;
};

} // namespace cheslib
