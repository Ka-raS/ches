#pragma once

#include <cassert>
#include <cstdint>

#include "cheslib/types.hpp"

namespace cheslib {

enum CastleFlag : uint8_t {
    NoCastles = 0,

    WhiteShortCastles = 0b0001,
    WhiteLongCastles = 0b0010,
    BlackShortCastles = 0b0100,
    BlackLongCastles = 0b1000,

    ShortCastles = WhiteShortCastles | BlackShortCastles,
    LongCastles = WhiteLongCastles | BlackLongCastles,
    WhiteCastles = WhiteShortCastles | WhiteLongCastles,
    BlackCastles = BlackShortCastles | BlackLongCastles,
    BothCastles = WhiteCastles | BlackCastles,
};

/**
 * 16bit encoded state
 * bit layout:
 *  0    : can white short castle
 *  1    : can white long castle
 *  2    : can black short castle
 *  3    : can black long castle
 *  4-7  : en passant file, bit 4 is has_en_passant
 *  8    : is black's turn
 *  9-15 : rule50 counter, counts to 100 moves for both side
 * where did I even find this
 */
class State {
  public:
    constexpr State();
    constexpr State(CastleFlag flag, File en_passant, Side side_to_move, int rule50_count);
    static constexpr State initial();

    constexpr bool operator==(const State &) const = default;

    constexpr CastleFlag castle_flag() const;
    constexpr bool can_castles(CastleFlag flag) const;
    constexpr void set_castles(CastleFlag flag);

    constexpr bool has_en_passant() const;
    constexpr File en_passant() const;
    constexpr void set_en_passant(File file);

    constexpr Side side_to_move() const;
    constexpr void switch_side();

    constexpr int rule50_count() const;
    constexpr void increment_rule50();
    constexpr void reset_rule50();

  private:
    uint16_t _data;
};

constexpr State::State() : _data(FileCNT << 4) { // en croissant
}

constexpr State::State(CastleFlag flag, File en_passant, Side side_to_move, int rule50_count)
    : _data(flag | (en_passant << 4) | ((side_to_move == Black) << 8) | (rule50_count << 9)) {
    assert(flag <= BothCastles);
    assert(en_passant <= FileCNT);
    assert(rule50_count <= 100);
}

constexpr State State::initial() {
    return State(BothCastles, FileCNT, White, 0);
}

constexpr CastleFlag State::castle_flag() const {
    return CastleFlag(_data & 0b1111);
}

constexpr bool State::can_castles(CastleFlag flag) const {
    assert(flag <= BothCastles);
    return _data & flag;
}

constexpr void State::set_castles(CastleFlag flag) {
    assert(flag <= BothCastles);
    _data = (_data & ~0b1111) | flag;
}

constexpr bool State::has_en_passant() const {
    return _data & (1 << 4);
}

constexpr File State::en_passant() const {
    return File((_data >> 4) & 0b1111);
}

constexpr void State::set_en_passant(File file) {
    assert(file <= FileCNT);
    constexpr uint16_t mask = 0b1111 << 4;
    _data = (_data & ~mask) | (file << 4);
}

constexpr Side State::side_to_move() const {
    return Side((_data >> 8) & 1);
}

constexpr void State::switch_side() {
    _data ^= 1 << 8;
}

constexpr int State::rule50_count() const {
    return _data >> 9;
}

constexpr void State::increment_rule50() {
    assert(rule50_count() < 120);
    _data += 1 << 9;
}

constexpr void State::reset_rule50() {
    _data &= 0b1'1111'1111;
}

} // namespace cheslib
