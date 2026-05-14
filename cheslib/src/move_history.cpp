#include <cstring>

#include "move_history.hpp"

namespace cheslib {

MoveHistory::MoveHistory()
    : _size(0) {}

bool MoveHistory::is_threefold_repetition(ZobristKey current) const {
    int count = 1;

    for (int i = (int)_size - 2; i >= 0; i -= 2) {
        if (_entries[i].key == current) {
            ++count;

            if (count == 3) {
                return true;
            }
        }
    }

    return false;
}

void MoveHistory::push(Entry entry) {
    assert(_size < MaxSize);
    _entries[_size] = entry;
    ++_size;
}

MoveHistory::Entry MoveHistory::pop() {
    assert(_size > 0);
    --_size;
    return _entries[_size];
}

void MoveHistory::trim() {
    assert(_size == 0 || _entries[0].state.rule50_count() == 0); // first move resets 50 moves rule

    size_t second_reset = _size;
    for (size_t i = 1; i < _size; ++i) {
        if (_entries[i].state.rule50_count() == 0) {
            second_reset = i;
            break;
        }
    }

    if (second_reset == _size) {
        return;
    }

    _size -= second_reset;
    assert(_size < 100); // size < 50 moves rule
    std::memmove(_entries, _entries + second_reset, _size * sizeof(Entry));
}

} // namespace cheslib
