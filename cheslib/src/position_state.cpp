#include "position_state.hpp"

namespace cheslib {

PositionState::PositionState(CastleFlag flag, File en_passant, Side side_to_move, int rule50_count)
    : _data(flag | (en_passant << 4) | ((side_to_move == Black) << 8) | (rule50_count << 9)) {
    assert(flag < CastleFlagCNT);
    assert(en_passant <= FileCNT);
    assert(rule50_count <= 100);
}

PositionState PositionState::initial() {
    return PositionState(BothCastles, FileCNT, White, 0);
}

CastleFlag PositionState::castle_flag() const {
    return CastleFlag(_data & 0b1111);
}

bool PositionState::can_castles(CastleFlag flag) const {
    assert(flag < CastleFlagCNT);
    return _data & flag;
}

void PositionState::revoke_castles(CastleFlag revoke) {
    assert(revoke < CastleFlagCNT);
    _data &= ~revoke;
}

bool PositionState::has_en_passant() const {
    return _data & (1 << 4);
}

File PositionState::en_passant() const {
    return File((_data >> 4) & 0b1111);
}

void PositionState::set_en_passant(File file) {
    assert(file <= FileCNT);
    constexpr uint16_t mask = 0b1111 << 4;
    _data = (_data & ~mask) | (file << 4);
}

Side PositionState::side_to_move() const {
    return Side((_data >> 8) & 1);
}

void PositionState::switch_side() {
    _data ^= 1 << 8;
}

unsigned PositionState::rule50_count() const {
    return _data >> 9;
}

void PositionState::increment_rule50() {
    assert(rule50_count() < 100);
    _data += 1 << 9;
}

void PositionState::reset_rule50() {
    _data &= 0b1'1111'1111;
}

} // namespace cheslib
