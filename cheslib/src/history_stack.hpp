#pragma once

#include "cheslib/move.hpp"

#include "zobrist.hpp"

namespace cheslib {

struct HistoryEntry {
    ZobristKey key;
    Move move;
    State state;
    Piece captured;
};

class HistoryStack {
  public:
    HistoryStack();
    HistoryEntry pop();
    void add(HistoryEntry &&entry);

  private:
    HistoryEntry _entries[512];
    std::size_t _size;
};

} // namespace cheslib
