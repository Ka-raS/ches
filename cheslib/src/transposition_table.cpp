#include "transposition_table.hpp"

namespace cheslib {

TranspositionTable::TranspositionTable()
    : _entries{} {}

void TranspositionTable::add(Entry entry) {
    _entries[index(entry.key)] = entry;
}

TranspositionTable::Entry TranspositionTable::get(ZobristKey key) const {
    Entry entry = _entries[index(key)];
    if (entry.key == key) {
        return entry;
    }
    return Entry{};
}

size_t TranspositionTable::index(ZobristKey key) {
    return key & (Size - 1);
}

} // namespace cheslib
