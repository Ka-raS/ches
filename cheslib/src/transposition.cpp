#include "transposition.hpp"

#include <algorithm>

namespace cheslib {

namespace {

constexpr Score MateThreshold = MateScore - MaxDepth;

unsigned calculate_key_shift(unsigned size_kib) {
    size_kib = std::clamp(size_kib, 1u, (4u * 1024u * 1024u)); // [1 KiB, 4 GiB]
    size_kib = std::bit_floor(size_kib);                       // round down to nearest power of 2

    const unsigned entries = size_kib * (1024u / sizeof(Transposition));
    return 64u - std::countr_zero(entries);
}

} // namespace

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

TranspositionTable::TranspositionTable(const unsigned size_kib) :
    _key_shift{calculate_key_shift(size_kib)},
    _entries{std::make_unique<std::atomic<Transposition>[]>(1u << (64 - _key_shift))} {}

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
    const size_t size = 1u << (64 - _key_shift);
    std::atomic<Transposition> *const entries = _entries.get();
    for (size_t i = 0; i < size; ++i) {
        entries[i].store(Transposition{}, std::memory_order::relaxed);
    }
}

size_t TranspositionTable::index(const ZobristKey key) const {
    return key >> _key_shift;
}

} // namespace cheslib
