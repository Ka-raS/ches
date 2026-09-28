#include "transposition.hpp"

namespace cheslib {

namespace {

constexpr Score MateThreshold = MateScore - MaxDepth;

}

Transposition::Transposition(
    const ZobristKey key, const Move move, const Score score, const Bound bound, const unsigned depth
) :
    _data(depth | (bound << 4) | uint32_t(key << 6)),
    _move{move},
    _score(score) {
    assert(depth <= MaxDepth);
    assert(bound <= BoundUpper);
}

bool Transposition::is_match(const ZobristKey key) const {
    constexpr uint64_t low26 = (1 << 26) - 1;
    return (key & low26) == (_data >> 6);
}

Move Transposition::move() const {
    return _move;
}

Score Transposition::score(const unsigned ply) const {
    // mate in n -> mate in (n + ply)
    if (_score >= MateThreshold) {
        return _score - ply;
    }
    if (_score <= -MateThreshold) {
        return _score + ply;
    }

    return _score;
}

Bound Transposition::bound() const {
    return Bound((_data >> 4) & 0b11);
}

unsigned Transposition::depth() const {
    return _data & 0b1111;
}

//

void TranspositionTable::store(
    const ZobristKey key, const Move move, Score score, const Bound bound, const unsigned depth, const unsigned ply
) {
    std::atomic<Transposition> &entry = _entries[index(key)];
    const unsigned current_depth = entry.load(std::memory_order::relaxed).depth();
    if (depth >= current_depth) {

        // mate in (n + ply) -> mate in n
        if (score >= MateThreshold) {
            score += ply;
        } else if (score <= -MateThreshold) {
            score -= ply;
        }

        entry.store(Transposition{key, move, score, bound, depth}, std::memory_order::relaxed);
    }
}

Transposition TranspositionTable::get(const ZobristKey key) const {
    return _entries[index(key)].load(std::memory_order::relaxed);
}

void TranspositionTable::reset() {
    for (std::atomic<Transposition> &entry : _entries) {
        entry.store(Transposition{}, std::memory_order::relaxed);
    }
}

size_t TranspositionTable::index(const ZobristKey key) {
    return key >> (64 - 20);
}

} // namespace cheslib
