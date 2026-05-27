#pragma once

#include <memory>
#include <vector>

#include "history_heuristic.hpp"
#include "position.hpp"
#include "thread.hpp"
#include "transposition.hpp"

namespace cheslib {

class Negamax {
  public:
    Negamax(unsigned search_depth, int threads_requested);
    void reset();
    void start_search(const Position &position, const Array<Move, 256> &legal_moves);
    bool is_searching() const;
    Move result() const;

  private:
    MoveScore iterative_deepening(Position &position, std::vector<MoveScore> &legal_moves);
    Score negamax(Position &position, uint8_t depth, Score alpha, Score beta);
    Score score_move(Move move, const Position &position) const; ///< for move ordering

    static size_t calculate_thread_count(int requested);

  private:
    const unsigned _max_depth;
    HistoryHeuristic _heuristics;
    std::unique_ptr<TranspositionTable> _transpositions;
    std::atomic<MoveScore> _result;
    std::vector<Thread> _threads;
};

} // namespace cheslib
