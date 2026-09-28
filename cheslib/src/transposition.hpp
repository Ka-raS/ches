#pragma once

#include "cheslib/move.hpp"
#include "types.hpp"

#include <atomic>

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
    TranspositionTable() = default;

    /// @return `Transposition` entry without checking `Transposition::is_match()`
    Transposition get(ZobristKey key) const;
    void store(ZobristKey key, Move move, Score score, Bound bound, unsigned depth, unsigned ply);
    void reset();

  private:
    static size_t index(ZobristKey key);

  private:
    std::atomic<Transposition> _entries[1 << 20] = {};
    static_assert(std::atomic<Transposition>::is_always_lock_free);
};

} // namespace cheslib
