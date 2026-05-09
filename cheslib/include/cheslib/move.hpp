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
    constexpr Move() = default;

    constexpr Move(Square from, Square to, MoveFlag flag) : _data(from | (to << 6) | (flag << 12)) {
        assert(from < SquareCNT);
        assert(to < SquareCNT);
        assert(flag <= QueenPromoCap && flag != 6 && flag != 7);
    }

    constexpr uint16_t data() const {
        return _data;
    }

    constexpr Square from() const {
        return Square(_data & 0b11'1111);
    }

    constexpr Square to() const {
        return Square((_data >> 6) & 0b11'1111);
    }

    constexpr MoveFlag flag() const {
        return MoveFlag((_data >> 12) & 0b1111);
    }

    constexpr bool is_promotion() const {
        return _data & (1 << 15);
    }

    constexpr bool is_capture() const {
        return _data & (1 << 14);
    }

    constexpr bool operator==(const Move &) const = default;

    constexpr PieceType promoted_piece() const {
        assert(is_promotion());

        PieceType type = PieceType(Knight + ((_data >> 12) & 0b11));

        assert(Knight <= type && type <= Queen);
        return type;
    }

  private:
    uint16_t _data;
};

} // namespace cheslib
