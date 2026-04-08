#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

#include "cheslib/types.hpp"

namespace ches {

enum MoveFlag : uint8_t {
    QuietMove = 0,
    DoublePawnPush = 1,
    ShortCastle = 2,
    LongCastle = 3,
    Capture = 4,
    EnPassant = 5,

    KnightPromo = 8,
    BishopPromo = 9,
    RookPromo = 10,
    QueenPromo = 11,
    KnightPromoCap = 12,
    BishopPromoCap = 13,
    RookPromoCap = 14,
    QueenPromoCap = 15
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
    constexpr Move() : _data(0) {
    }

    constexpr Move(Square from, Square to, MoveFlag flag) : _data(from | (to << 6) | (flag << 12)) {
        assert(from < SquareCNT);
        assert(to < SquareCNT);
        assert(flag <= QueenPromoCap);
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

    constexpr bool isPromotion() const {
        return flag() & 0b1000;
    }

    constexpr PieceType promotion_piece() const {
        MoveFlag f = flag();
        bool is_promo = f & 0b1000;
        return is_promo ? PieceType((f & 0b11) + Knight) : PieceTypeCNT;
    }

    constexpr bool isCapture() const {
        return flag() & 0b0100;
    }

    constexpr bool operator==(Move other) const {
        return _data == other._data;
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

    constexpr size_t size() const {
        return _end - _moves;
    }

    template <typename... Args>
    constexpr void add(Args &&...args) {
        assert(size() < 256); // not happening
        *_end++ = Move(std::forward<Args>(args)...);
    }

    constexpr bool has(Move target) const {
        for (Move move : *this) {
            if (move == target) {
                return true;
            }
        }
        return false;
    }

  private:
    Move _moves[256];
    Move *_end;
};

} // namespace ches
