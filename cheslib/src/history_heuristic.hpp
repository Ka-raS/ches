#pragma once

#include <atomic>

#include "types.hpp"

namespace cheslib {

class HistoryHeuristic {
  public:
    HistoryHeuristic();
    Score get(Piece piece, Square to) const;
    void update(Piece piece, Square to, uint8_t depth);
    void reset();

  private:
    std::atomic_int16_t _entries[PieceCNT][SquareCNT];
    static_assert(std::atomic_int16_t::is_always_lock_free);
};

} // namespace cheslib
