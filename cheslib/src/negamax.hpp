#pragma once

#include "history_heuristic.hpp"
#include "position.hpp"
#include "thread.hpp"
#include "transposition.hpp"

namespace cheslib {

class Negamax {
  public:
    Negamax(unsigned search_depth, int thread_count);

    unsigned search_depth() const;
    unsigned thread_count() const;
    void set_search_depth(unsigned search_depth);
    void set_thread_count(int thread_count);

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
    int16_t scoring(Move move, const Pieces &pieces) const; ///< for move ordering

    /**
     * @param alpha lower bound of score
     * @param beta upper bound of score
     * @param depth remaining depth to search
     * @param ply distance from root node
     */
    Score negamax(Position &position, Score alpha, Score beta, unsigned depth, unsigned ply);

  private:
    std::unique_ptr<TranspositionTable> _transpositions;
    HistoryHeuristic _heuristics;

    std::atomic<MoveScore> _result;
    std::atomic_bool _stop;
    uint8_t _search_depth;
    uint8_t _thread_count;

    std::unique_ptr<Thread[]> _threads;

    static_assert(std::atomic<MoveScore>::is_always_lock_free);
};

} // namespace cheslib
