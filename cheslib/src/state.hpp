#pragma once

#include <cassert>
#include <cstdint>

#include "cheslib/types.hpp"

#include "utils.hpp"

namespace ches {

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
 *  4-7  : en passant file
 *  8    : is black's turn
 *  9-15 : rule50 counter
 * where did I even find this
 */
class State {
  public:
    constexpr State() : _data(0) {
    }

    constexpr State(CastleFlag flag, File en_passant, Side side_to_move, int rule50_count)
        : _data(flag | (en_passant << 4) | ((side_to_move == Black) << 8) | (rule50_count << 9)) {
        assert(flag <= BothCastles);
        assert(en_passant <= FileCNT);
        assert(rule50_count <= 100);
    }

    static constexpr State initial() {
        return State(BothCastles, FileCNT, White, 0);
    }

    constexpr bool operator==(State other) const {
        return _data == other._data;
    }

    constexpr CastleFlag castle_flag() const {
        return CastleFlag(_data & 0b1111);
    }

    constexpr bool can_castles(CastleFlag flag) const {
        assert(flag <= BothCastles);
        return _data & flag;
    }

    constexpr void set_castles(CastleFlag flag) {
        assert(flag <= BothCastles);
        _data = (_data & ~0b1111) | flag;
    }

    constexpr File en_passant() const {
        return File((_data >> 4) & 0b1111);
    }

    constexpr Side side_to_move() const {
        return Side((_data >> 8) & 1);
    }

    constexpr int rule50_count() const {
        return _data >> 9;
    }

    constexpr void set_en_passant(File file) {
        assert(file <= FileCNT);
        constexpr uint16_t mask = 0b1111 << 4;
        _data = (_data & ~mask) | (file << 4);
    }

    constexpr void increment_rule50() {
        assert(rule50_count() < 120);
        _data += 1 << 9;
    }

    constexpr void reset_rule50() {
        _data &= 0b1'1111'1111;
    }

    constexpr void switch_side() {
        _data ^= 1 << 8;
    }

  private:
    uint16_t _data;
};

} // namespace ches
