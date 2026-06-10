#include "transposition.hpp"

namespace cheslib {

Transposition::Transposition(const ZobristKey key, const MoveScore move_score, const Bound bound, const unsigned depth)
    : _data{depth | (uint32_t(bound) << 4) | (encode(key) << 6)},
      _move_score{move_score} {}

bool Transposition::is_match(const ZobristKey key) const {
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

uint32_t Transposition::encode(const ZobristKey key) {
    return (key * 0x94D049BB133111EB) >> (64 - 26);
}

//

void TranspositionTable::store(
    const ZobristKey key, const MoveScore move_score, const Bound bound, const unsigned depth
) {
    std::atomic<Transposition> &entry = _entries[index(key)];
    const unsigned current_depth = entry.load(std::memory_order_acquire).depth();

    if (depth >= current_depth) {
        entry.store(Transposition{key, move_score, bound, depth}, std::memory_order_release);
    }
}

Transposition TranspositionTable::get(const ZobristKey key) const {
    return _entries[index(key)].load(std::memory_order_acquire);
}

void TranspositionTable::reset() {
    for (std::atomic<Transposition> &entry : _entries) {
        entry.store(Transposition{}, std::memory_order_release);
    }
}

size_t TranspositionTable::index(const ZobristKey key) {
    return (key * 0xBF58476D1CE4E5B9) >> (64 - 20);
}

} // namespace cheslib
