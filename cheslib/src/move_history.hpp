#pragma once

#include "cheslib/move.hpp"

#include "state.hpp"

namespace cheslib {

class MoveHistory {
  public:
    static constexpr size_t MaxSize = 256;
    struct Entry {
        ZobristKey key;
        Move move;
        State state;
        Piece captured;
    };

  public:
    MoveHistory();

    bool is_threefold_repetition(ZobristKey current) const;

    void push(Entry entry);
    Entry pop();
    void trim();

  private:
    Entry _entries[MaxSize];
    size_t _size;
};

} // namespace cheslib
