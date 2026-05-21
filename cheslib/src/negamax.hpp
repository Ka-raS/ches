#pragma once

#include <memory>
#include <vector>

#include "position.hpp"
#include "thread.hpp"
#include "transposition_table.hpp"

namespace cheslib {

class Negamax {
  public:
    Negamax(size_t thread_count);
    void start_search(const Position &position, const Array<Move, 256> &legal_moves);
    bool is_searching() const;
    Move result() const;

  private:
    MoveScore iterative_deepening(Position &position, std::vector<MoveScore> &legal_moves);
    Score negamax(Position &position, unsigned depth, Score alpha, Score beta);
    Score scoring_move(Move move, const Position &position) const;

  private:
    Score _history_heuristic[PieceCNT][SquareCNT];
    std::unique_ptr<TranspositionTable> _transposition_table;
    std::atomic<MoveScore> _result;
    std::vector<Thread> _threads;
};

} // namespace cheslib
