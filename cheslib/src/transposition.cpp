#include "transposition.hpp"

namespace cheslib {

Transposition::Transposition()
    : _data(0) {}

Transposition::Transposition(ZobristKey key, MoveScore move_score, Bound bound, unsigned depth)
    : _data(depth | (uint32_t(bound) << 4) | (encode(key) << 6)),
      _move_score(move_score) {}

bool Transposition::is_match(ZobristKey key) const {
    return encode(key) == (_data >> 6);
}

Move Transposition::move() const {
    return _move_score.move;
}

Score Transposition::score() const {
    return _move_score.score;
}

Bound Transposition::bound() const {
    return Bound((_data >> 4) & 0b11);
}

unsigned Transposition::depth() const {
    return _data & 0b1111;
}

uint32_t Transposition::encode(ZobristKey key) {
    return (key * 0x94D049BB133111EB) >> (64 - 26);
}

TranspositionTable::TranspositionTable()
    : _entries{} {}

void TranspositionTable::store(ZobristKey key, MoveScore move_score, Bound bound, unsigned depth) {
    std::atomic<Transposition> &entry = _entries[index(key)];
    Transposition current = entry.load(std::memory_order_relaxed);

    if (bound == Bound::Exact || !current.is_match(key) || depth > current.depth()) {
        entry.store(Transposition(key, move_score, bound, depth), std::memory_order_relaxed);
    }
}

Transposition TranspositionTable::get(ZobristKey key) const {
    return _entries[index(key)].load(std::memory_order_relaxed);
}

void TranspositionTable::reset() {
    for (std::atomic<Transposition> &entry : _entries) {
        entry.store(Transposition(), std::memory_order_relaxed);
    }
}

size_t TranspositionTable::index(ZobristKey key) {
    return (key * 0xBF58476D1CE4E5B9) >> (64 - 20);
}

} // namespace cheslib
