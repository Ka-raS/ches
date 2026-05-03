#pragma once

#include "cheslib/types.hpp"

namespace cheslib {

enum MoveFlag : uint8_t {
    QuietMove = 0,
    DoublePawnPush = 0b001,
    ShortCastle = 0b010,
    LongCastle = 0b011,
    Capture = 0b100,
    EnPassant = 0b101,

    KnightPromo = 0b1000,
    BishopPromo = 0b1001,
    RookPromo = 0b1010,
    QueenPromo = 0b1011,

    KnightPromoCap = KnightPromo | Capture,
    BishopPromoCap = BishopPromo | Capture,
    RookPromoCap = RookPromo | Capture,
    QueenPromoCap = QueenPromo | Capture
};

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
