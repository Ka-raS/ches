#pragma once

#include "history_heuristic.hpp"
#include "position.hpp"
#include "thread.hpp"
#include "transposition.hpp"

#include <memory>
#include <vector>

namespace cheslib {

class Negamax {
  public:
    Negamax(unsigned search_depth, int thread_count);

    void start_search(const Position &position, const Array<Move, 256> &legal_moves);
    void stop_search();
    void wait_while_searching() const;
    bool is_searching() const;
    Move result() const;
    void reset();

  private:
    struct RootNode {
        Position position;
        Array<MoveScore, 256> legal_moves;
    };

    MoveScore iterative_deepening(RootNode root);
    Score negamax(Position &position, unsigned depth, Score alpha, Score beta);
    Score score_move(Move move, const Position &position) const; ///< for move ordering

  private:
    std::unique_ptr<TranspositionTable> _transpositions;
    HistoryHeuristic _heuristics;

    const unsigned _max_depth; // TODO: allow modifying
    std::atomic<MoveScore> _result;
    std::atomic_bool _stop;

    std::vector<Thread> _threads;
};

} // namespace cheslib
