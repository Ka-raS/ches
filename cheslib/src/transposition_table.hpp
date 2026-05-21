#pragma once

#include "cheslib/move.hpp"

#include "types.hpp"

namespace cheslib {

class TranspositionTable {
  public:
    struct Entry {
        ZobristKey key;
        Move move;
        Score score;
        Bound bound;
        uint8_t depth;
    };

  public:
    TranspositionTable();
    void add(Entry entry);
    Entry get(ZobristKey key) const;

  private:
    static size_t index(ZobristKey key);

  private:
    static constexpr size_t Size = 1 << 20;
    Entry _entries[Size];
};

} // namespace cheslib
