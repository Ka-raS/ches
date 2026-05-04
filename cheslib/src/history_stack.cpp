#include "history_stack.hpp"

namespace cheslib {

HistoryStack::HistoryStack() : _entries{}, _size(0) {
}

HistoryEntry HistoryStack::pop() {
    assert(_size > 0);
    --_size;
    return std::move(_entries[_size]);
}

void HistoryStack::add(HistoryEntry &&entry) {
    assert(_size < 512);
    _entries[_size] = std::move(entry);
    ++_size;
}

} // namespace cheslib
