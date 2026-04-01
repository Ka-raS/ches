#pragma once

#include <cassert>
#include <cstdint>

#include "cheslib/types.hpp"

namespace cheslib {

enum MoveFlag : uint8_t {
    QUIET_MOVE = 0,
    DOUBLE_PAWN_PUSH = 1,
    KING_CASTLE = 2,
    QUEEN_CASTLE = 3,
    CAPTURE = 4,
    EN_PASSANT = 5,
    KNIGHT_PROMOTION = 8,
    BISHOP_PROMOTION = 9,
    ROOK_PROMOTION = 10,
    QUEEN_PROMOTION = 11,
    KNIGHT_PROMO_CAPTURE = 12,
    BISHOP_PROMO_CAPTURE = 13,
    ROOK_PROMO_CAPTURE = 14,
    QUEEN_PROMO_CAPTURE = 15
};

/**
 * 16bit encoded move
 * bit layout:
 *  0-5   : from square
 *  6-11  : to square
 *  12-13 : promotion piece
 *  14    : is capture
 *  15    : is promotion
 * see more here: https://www.chessprogramming.org/Encoding_Moves
 */
class Move {
  public:
    constexpr Move(Square from, Square to, MoveFlag flag) : _data(from | (to << 6) | (flag << 12)) {
        assert(from < SQUARE_CNT);
        assert(to < SQUARE_CNT);
        assert(flag <= QUEEN_PROMO_CAPTURE);
    }

    ~Move() = default;

    constexpr Square from() const {
        return Square(_data & 0b111111);
    }

    constexpr Square to() const {
        return Square((_data >> 6) & 0b111111);
    }

    constexpr MoveFlag flag() const {
        return MoveFlag((_data >> 12) & 0b1111);
    }

    constexpr bool is_promotion() const {
        return flag() & 0b1000;
    }

    constexpr Piece promotion_piece() const {
        assert(is_promotion());
        return Piece(flag() & 0b11);
    }

    constexpr bool isCapture() const {
        return flag() & 0b0100;
    }

    constexpr bool isCastle() const {
        MoveFlag f = flag();
        return f == KING_CASTLE || f == QUEEN_CASTLE;
    }

  private:
    uint16_t _data;
};

} // namespace cheslib
