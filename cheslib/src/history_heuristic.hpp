#pragma once

#include "position.hpp"
#include "types.hpp"

#include <atomic>

namespace cheslib {

/// heuristic for quiet moves
class HistoryHeuristic {
  public:
    HistoryHeuristic() = default;

    void reset();
    Score get(Piece piece, Square to) const;

    /**
     * update heuristics of seached moves
     * @param back the move that caused cutoff
     */
    void update(const MoveScore *front, const MoveScore *back, const Pieces &pieces, Score bonus);

  private:
    std::atomic_int16_t _scores[PieceCNT][SquareCNT] = {0};
    static_assert(std::atomic_int16_t::is_always_lock_free);
};

} // namespace cheslib
