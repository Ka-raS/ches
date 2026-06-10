#pragma once

#include <atomic>

#include "position.hpp"
#include "types.hpp"

namespace cheslib {

class HistoryHeuristic {
  public:
    Score get(Piece piece, Square to) const;
    void update(const Position &position, Move move, unsigned depth);
    void reset();

  private:
    std::atomic_int16_t _scores[PieceCNT][SquareCNT] = {0};
    static_assert(std::atomic_int16_t::is_always_lock_free);
};

} // namespace cheslib
