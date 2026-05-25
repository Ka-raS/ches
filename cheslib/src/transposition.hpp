#pragma once

#include <atomic>

#include "cheslib/move.hpp"

#include "types.hpp"

namespace cheslib {

class Transposition {
  public:
    Transposition();
    Transposition(ZobristKey key, MoveScore move_score, Bound bound, unsigned depth);

    bool is_match(ZobristKey key) const;
    Move move() const;
    Score score() const;
    Bound bound() const;
    unsigned depth() const;

  private:
    static uint32_t encode(ZobristKey key);

  private:
    uint32_t _data; // 4bit depth, 2bit Bound, 26bit encoded ZobristKey
    MoveScore _move_score;
};

class TranspositionTable {
  public:
    TranspositionTable();

    /// @return `Transposition` entry without checking `Transposition::is_match()`
    Transposition get(ZobristKey key) const;
    void store(ZobristKey key, MoveScore move_score, Bound bound, unsigned depth);
    void reset();

  private:
    static size_t index(ZobristKey key);

  private:
    std::atomic<Transposition> _entries[1 << 20];
    static_assert(std::atomic<Transposition>::is_always_lock_free);
};

} // namespace cheslib
