#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

#include "cheslib/types.hpp"

namespace cheslib {

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
 *
 *  12-13 : special bits, if promotion move then it's promoted piece type
 *  14    : is capture
 *  15    : is promotion
 * see: https://www.chessprogramming.org/Encoding_Moves
 */
class Move {
  public:
    constexpr Move();
    constexpr Move(Square from, Square to, MoveFlag flag);
    constexpr Square from() const;
    constexpr Square to() const;
    constexpr MoveFlag flag() const;
    constexpr PieceType promo_piece() const;
    constexpr bool is_promotion() const;
    constexpr bool is_capture() const;
    constexpr bool operator==(const Move &) const = default;

  private:
    uint16_t _data;
};

class MoveList {
  public:
    constexpr MoveList();
    constexpr size_t size() const;
    constexpr const Move *begin() const;
    constexpr const Move *end() const;
    constexpr bool has(Move target) const;
    constexpr void add(Square from, Square to, MoveFlag flag);
    constexpr Move operator[](size_t idx) const;

  private:
    static constexpr size_t s_size = 256;
    Move _moves[s_size];
    size_t _size;
};

constexpr Move::Move() : _data(0) {
}

constexpr Move::Move(Square from, Square to, MoveFlag flag) : _data(from | (to << 6) | (flag << 12)) {
    assert(from < SquareCNT);
    assert(to < SquareCNT);
    assert(flag <= QueenPromoCap);
}

constexpr Square Move::from() const {
    return Square(_data & 0b11'1111);
}

constexpr Square Move::to() const {
    return Square((_data >> 6) & 0b11'1111);
}

constexpr MoveFlag Move::flag() const {
    return MoveFlag((_data >> 12) & 0b1111);
}

constexpr bool Move::is_promotion() const {
    return flag() & 0b1000;
}

constexpr PieceType Move::promo_piece() const {
    assert(is_promotion());
    MoveFlag f = flag();
    return PieceType((f & 0b11) + Knight);
}

constexpr bool Move::is_capture() const {
    return flag() & 0b0100;
}

constexpr MoveList::MoveList() : _size(0) {
}

constexpr size_t MoveList::size() const {
    return _size;
}

constexpr const Move *MoveList::begin() const {
    return _moves;
}

constexpr const Move *MoveList::end() const {
    return _moves + _size;
}

constexpr Move MoveList::operator[](size_t idx) const {
    assert(idx < _size);
    return _moves[idx];
}

constexpr bool MoveList::has(Move target) const {
    for (size_t i = 0; i < _size; ++i) {
        if (_moves[i] == target) {
            return true;
        }
    }
    return false;
}

constexpr void MoveList::add(Square from, Square to, MoveFlag flag) {
    assert(size() < s_size);
    _moves[_size] = Move(from, to, flag);
    ++_size;
}

} // namespace cheslib
