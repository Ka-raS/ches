#pragma once

#include "cheslib/move.hpp"
#include "types.hpp"

#include <atomic>
#include <memory>

namespace cheslib {

class alignas(8) Transposition {
  public:
    Transposition() = default;
    Transposition(ZobristKey key, Move move, Score score, Bound bound, unsigned depth);

    Move move() const;
    Score score(unsigned ply) const;
    Bound bound() const;
    unsigned depth() const;
    bool is_match(ZobristKey key) const;

  private:
    uint32_t _data; // 4bit depth, 2bit Bound, 26bit ZobristKey
    Move _move;
    int16_t _score;
};

class TranspositionTable {
  public:
    TranspositionTable(unsigned size_kib);

    /// @return `Transposition` entry without checking `Transposition::is_match()`
    Transposition get(ZobristKey key) const;
    void store(ZobristKey key, Move move, Score score, Bound bound, unsigned depth, unsigned ply);
    void reset();

  private:
    size_t index(ZobristKey key) const;

  private:
    unsigned _key_shift; /// `1 << (64 - _key_shift)` is the size of `_entries`
    std::unique_ptr<std::atomic<Transposition>[]> _entries;

    static_assert(std::atomic<Transposition>::is_always_lock_free);
};

} // namespace cheslib
