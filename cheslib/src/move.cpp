#include "cheslib/move.hpp"

namespace cheslib {

uint16_t Move::data() const {
    return _data;
}

Square Move::from() const {
    return Square(_data & 0b11'1111);
}

Square Move::to() const {
    return Square((_data >> 6) & 0b11'1111);
}

MoveFlag Move::flag() const {
    return MoveFlag((_data >> 12) & 0b1111);
}

bool Move::is_promotion() const {
    return _data & (1 << 15);
}

PieceType Move::promo_piece() const {
    unsigned encoded_piece = (_data >> 12) & 0b11;
    return PieceType(encoded_piece + Knight);
}

bool Move::is_capture() const {
    return _data & (1 << 14);
}

} // namespace cheslib
