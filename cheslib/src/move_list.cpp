#include "cheslib/move_list.hpp"

namespace cheslib {

MoveList::MoveList() : _moves{}, _size(0) {
}

std::size_t MoveList::size() const {
    return _size;
}

const Move *MoveList::begin() const {
    return _moves;
}

const Move *MoveList::end() const {
    return _moves + _size;
}

const Move *MoveList::find(Square from, Square to) const {
    constexpr uint16_t mask = 0xFFFu;
    const uint16_t target = from | (to << 6);

    for (const Move &move : *this) {
        if ((move.data() & mask) == target) {
            return &move;
        }
    }

    return nullptr;
}

bool MoveList::has(Move move) const {
    for (Move m : *this) {
        if (m == move) {
            return true;
        }
    }
    return false;
}

void MoveList::add(Move move) {
    assert(_size < MaxSize);
    _moves[_size] = move;
    ++_size;
}

} // namespace cheslib