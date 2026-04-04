#pragma once

#include <cassert>
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
 *  8    : is black's turn
 *  9-15 : rule50 counter
 * where did I even find this
 */
class State {
  public:
    constexpr State(
        bool can_white_short_castle, bool can_white_long_castle, bool can_black_short_castle,
        bool can_black_long_castle, File en_passant, bool is_black_turn, uint8_t rule50_count
    )
        : _data(
              (can_white_short_castle) | (can_white_long_castle << 1) | (can_black_short_castle << 2) |
              (can_black_long_castle << 3) | (en_passant << 4) | (is_black_turn << 8) | (rule50_count << 9)
          ) {
        assert(en_passant <= FILE_CNT);
        assert(rule50_count <= 100);
    }

    static constexpr State initial() {
        return State(true, true, true, true, FILE_CNT, false, 0);
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

    constexpr bool is_blacks_turn() const {
        return _data & (1 << 8);
    }

    constexpr uint8_t rule50_count() const {
        return _data >> 9;
    }

  private:
    uint16_t _data;
};

} // namespace cheslib
