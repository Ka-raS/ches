#pragma once

#include <cstdint>

#include "cheslib/types.hpp"

namespace cheslib {

/**
 * 16bit encoded state
 * bit layout:
 *  0    : can white short castle
 *  1    : can white long castle
 *  2    : can black short castle
 *  3    : can black long castle
 *  4-7  : en passant file
 *  8    : is white to move
 *  9-15 : rule50 counter
 * where did I even find this
 */
class State {
  public:
    static constexpr State initial() {
        State state{};
        state._data = 0b0000000100001111;
        return state;
    }

    constexpr bool can_white_short_castle() const {
        return _data & 1;
    }

    constexpr bool can_white_long_castle() const {
        return _data & 0b10;
    }

    constexpr bool can_black_short_castle() const {
        return _data & 0b100;
    }

    constexpr bool can_black_long_castle() const {
        return _data & 0b1000;
    }

    constexpr File en_passant() const {
        return File((_data >> 4) & 0b1111);
    }

    constexpr bool is_white_to_move() const {
        return _data & (1 << 8);
    }

    constexpr uint8_t rule50_count() const {
        return _data >> 9;
    }

  private:
    uint16_t _data;
};

} // namespace cheslib
