#pragma once

#include <cassert>
#include <cstdint>

#include "cheslib/types.hpp"

namespace cheslib {

enum MoveFlag : uint8_t {
    QUIET_MOVE = 0,
    DOUBLE_PAWN_PUSH = 1,
    SHORT_CASTLE = 2,
    LONG_CASTLE = 3,
    CAPTURE = 4,
    EN_PASSANT = 5,

    KNIGHT_PROMO = 8,
    BISHOP_PROMO = 9,
    ROOK_PROMO = 10,
    QUEEN_PROMO = 11,
    KNIGHT_PROMO_CAP = 12,
    BISHOP_PROMO_CAP = 13,
    ROOK_PROMO_CAP = 14,
    QUEEN_PROMO_CAP = 15
};

constexpr MoveFlag operator--(MoveFlag &flag) {
    return flag = MoveFlag(flag - 1);
}

/**
 * 16bit encoded move
 * bit layout:
 *  0-5   : from square
 *  6-11  : to square
 *  12-13 : promotion piece
 *  14    : is capture
 *  15    : is promotion
 * see: https://www.chessprogramming.org/Encoding_Moves
 */
class Move {
  public:
    constexpr Move() : _data(0) {};

    constexpr Move(Square from, Square to, MoveFlag flag) : _data(from | (to << 6) | (flag << 12)) {
        assert(from < SQUARE_CNT);
        assert(to < SQUARE_CNT);
        assert(flag <= QUEEN_PROMO_CAP);
    }

    constexpr Square from() const {
        return Square(_data & 0b111111);
    }

    constexpr Square to() const {
        return Square((_data >> 6) & 0b111111);
    }

    constexpr MoveFlag flag() const {
        return MoveFlag((_data >> 12) & 0b1111);
    }

    constexpr Piece promotion_piece() const {
        uint8_t f = flag();
        bool is_promo = f & 0b1000;
        return is_promo ? Piece(KNIGHT + f & 0b11) : PIECE_CNT;
    }

    constexpr bool isCapture() const {
        return flag() & 0b0100;
    }

    constexpr bool isCastle() const {
        MoveFlag f = flag();
        return f == SHORT_CASTLE || f == LONG_CASTLE;
    }

  private:
    uint16_t _data;
};

class MoveList {
  public:
    constexpr MoveList() : _end(_moves) {
    }

    constexpr const Move *begin() const {
        return _moves;
    }

    constexpr const Move *end() const {
        return _end;
    }

    constexpr bool is_empty() const {
        return _end == _moves;
    }

    constexpr void clear() {
        _end = _moves;
    }

    constexpr void add(Square from, Square to, MoveFlag flag) {
        *_end++ = Move(from, to, flag);
    }

  private:
    Move _moves[256];
    Move *_end;
};

} // namespace cheslib
