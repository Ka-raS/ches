#include "state.hpp"

namespace cheslib {

State::State(CastleFlag flag, File en_passant, Side side_to_move, int rule50_count)
    : _data(flag | (en_passant << 4) | ((side_to_move == Black) << 8) | (rule50_count << 9)) {
    assert(flag <= BothCastles);
    assert(en_passant <= FileCNT);
    assert(rule50_count <= 100);
}

State State::initial() {
    return State(BothCastles, FileCNT, White, 0);
}

CastleFlag State::castle_flag() const {
    return CastleFlag(_data & 0b1111);
}

bool State::can_castles(CastleFlag flag) const {
    assert(flag <= BothCastles);
    return _data & flag;
}

void State::revoke_castles(CastleFlag revoke) {
    assert(revoke <= BothCastles);
    _data &= ~revoke;
}

bool State::has_en_passant() const {
    return _data & (1 << 4);
}

File State::en_passant() const {
    return File((_data >> 4) & 0b1111);
}

void State::set_en_passant(File file) {
    assert(file <= FileCNT);
    constexpr uint16_t mask = 0b1111 << 4;
    _data = (_data & ~mask) | (file << 4);
}

Side State::side_to_move() const {
    return Side((_data >> 8) & 1);
}

void State::switch_side() {
    _data ^= 1 << 8;
}

int State::rule50_count() const {
    return _data >> 9;
}

void State::increment_rule50() {
    assert(rule50_count() < 100);
    _data += 1 << 9;
}

void State::reset_rule50() {
    _data &= 0b1'1111'1111;
}

} // namespace cheslib
