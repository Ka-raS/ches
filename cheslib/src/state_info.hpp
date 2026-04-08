#pragma once

#include <cassert>
#include <cstdint>

#include "cheslib/types.hpp"

#include "utils.hpp"

namespace ches {

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
class StateInfo {
  public:
    constexpr StateInfo(
        bool can_white_short_castle = false, bool can_white_long_castle = false, bool can_black_short_castle = false,
        bool can_black_long_castle = false, File en_passant = FileCNT, Side side_to_move = White, int rule50_count = 0
    )
        : _data(
              (can_white_short_castle) | (can_white_long_castle << 1) | (can_black_short_castle << 2) |
              (can_black_long_castle << 3) | (en_passant << 4) | ((side_to_move == Black) << 8) | (rule50_count << 9)
          ) {
        assert(en_passant <= FileCNT);
        assert(rule50_count <= 100);
    }

    static constexpr StateInfo initial() {
        return StateInfo(true, true, true, true, FileCNT, White, 0);
    }

    template <Side Us>
    constexpr bool can_short_castle() const {
        constexpr uint16_t mask = (Us == White) ? 0b1 : 0b100;
        return _data & mask;
    }

    template <Side Us>
    constexpr bool can_long_castle() const {
        constexpr uint16_t mask = (Us == White) ? 0b10 : 0b1000;
        return _data & mask;
    }

    template <Side Us>
    constexpr void revoke_short_castle() {
        constexpr uint16_t mask = (Us == White) ? 0b1 : 0b100;
        _data &= ~mask;
    }

    template <Side Us>
    constexpr void revoke_long_castle() {
        constexpr uint16_t mask = (Us == White) ? 0b10 : 0b1000;
        _data &= ~mask;
    }

    constexpr File ep_file() const {
        return File((_data >> 4) & 0b1111);
    }

    constexpr Square ep_square() const {
        File file = ep_file();
        if (file >= FileCNT) {
            return SquareCNT;
        }
        Rank rank = (side_to_move() == White) ? Rank6 : Rank3;
        return to_square(file, rank);
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
        int count = rule50_count();
        if (count < 100) {
            constexpr uint16_t mask = 0b1111111 << 9;
            _data = (_data & ~mask) | ((count + 1) << 9);
        }
    }

    constexpr void reset_rule50() {
        _data &= 0b1111111;
    }

    constexpr void switch_side() {
        _data ^= 1 << 8;
    }

  private:
    uint16_t _data;
};

} // namespace ches
