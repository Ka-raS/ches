#pragma once

#include "types.hpp"

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

    CastleFlagCNT = 16
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
class PositionState {
  public:
    PositionState() = default;
    PositionState(CastleFlag flag, File en_passant, Side side_to_move, int rule50_count);
    static PositionState initial();

    bool operator==(const PositionState &) const = default;

    CastleFlag castle_flag() const;
    bool can_castles(CastleFlag flag) const;
    void revoke_castles(CastleFlag revoke);

    bool has_en_passant() const;
    File en_passant() const;
    void set_en_passant(File file);

    Side side_to_move() const;
    void switch_side();

    unsigned rule50_count() const;
    void increment_rule50();
    void reset_rule50();

  private:
    uint16_t _data;
};

} // namespace cheslib
