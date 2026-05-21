#include <algorithm>

#include "eval.hpp"
#include "movegen.hpp"
#include "negamax.hpp"

namespace cheslib {

Negamax::Negamax(size_t thread_count)
    : _history_heuristic{},
      _transposition_table(std::make_unique<TranspositionTable>()),
      _threads(thread_count) {
    assert(thread_count > 0);
}

Move Negamax::result() const {
    assert(!is_searching());
    MoveScore result = _result.load(std::memory_order_relaxed);
    return result.move;
}

bool Negamax::is_searching() const {
    for (const Thread &thread : _threads) {
        if (thread.state() == Thread::State::Working) {
            return true;
        }
    }
    return false;
}

Score Negamax::scoring_move(Move move, const Position &position) const {
    Score score = 0;

    if (move.is_capture()) {
        Square to = move.to();
        Piece captured = position.pieces().at(to);
        // score += 10 * types::value_of(captured);

        // if (move.is_promotion()) {
        //     score += 10 * types::value_of(types::piece_of(position.state().side_to_move(), move.promoted_piece()));
        // }
    }

    Piece piece = position.pieces().at(move.from());
    Square to = move.to();
    score += _history_heuristic[piece][to];

    return score;
}

void Negamax::start_search(const Position &position, const Array<Move, 256> &legal_moves) {
    const size_t thread_count = _threads.size();

    auto sort_moves = [this, &position, &legal_moves]() -> Array<MoveScore, 256> {
        Array<MoveScore, 256> moves;

        for (Move move : legal_moves) {
            moves.push(move, scoring_move(move, position));
        }

        std::ranges::sort(moves, std::greater{}, &MoveScore::score);
        return moves;
    };

    auto assign_moves = [thread_count, sorted = sort_moves()](size_t thread_index) -> std::vector<MoveScore> {
        if (thread_index >= sorted.size()) {
            return {};
        }

        std::vector<MoveScore> moves;
        moves.reserve(1 + (sorted.size() - 1 - thread_index) / thread_count);

        for (size_t i = thread_index; i < sorted.size(); i += thread_count) {
            moves.push_back(sorted[i]);
        }

        return moves;
    };

    _result.store(MoveScore{.score = INT16_MIN}, std::memory_order_relaxed);

    for (size_t i = 0; i < thread_count; ++i) {
        _threads[i].assign_job([this, pos = position, assigned = assign_moves(i)]() mutable -> void {
            if (assigned.empty()) {
                return;
            }

            const MoveScore best = iterative_deepening(pos, assigned);
            MoveScore result = _result.load(std::memory_order_acquire);

            while ( // compare and swap loop
                best.score >= result.score &&
                !_result.compare_exchange_weak(result, best, std::memory_order_release, std::memory_order_acquire)) {}
        });
    }
}

MoveScore Negamax::iterative_deepening(Position &position, std::vector<MoveScore> &legal_moves) {
    assert(!legal_moves.empty());

    MoveScore best = {.score = INT16_MIN};

    for (unsigned depth = 0; depth < 6; ++depth) {
        const TranspositionTable::Entry entry = _transposition_table->get(position.key());

        if (entry.key == position.key() && entry.depth >= depth) {
            for (auto &[move, score] : legal_moves) {
                if (move == entry.move) {
                    score = entry.score;
                    break;
                }
            }
        }

        std::ranges::sort(legal_moves, std::greater{}, &MoveScore::score);

        for (MoveScore &current : legal_moves) {
            position.do_move(current.move);
            current.score = -negamax(position, depth, INT16_MIN, -best.score);
            position.undo_move();

            if (best.score < current.score) {
                best = current;
            }
        }
    }

    return best;
}

Score Negamax::negamax(Position &position, const unsigned depth, Score alpha, Score beta) {
    if (position.is_50move_draw() || position.is_3fold_repetition()) {
        return 0;
    }
    if (depth == 0) {
        return eval::evaluate(position);
    }

    TranspositionTable::Entry entry = _transposition_table->get(position.key());
    if (entry.key == position.key() && entry.depth >= depth) {
        switch (entry.bound) {
        case Bound::Exact:
            return entry.score;

        case Bound::Lower:
            alpha = std::max(alpha, entry.score);
            break;

        case Bound::Upper:
            beta = std::min(beta, entry.score);
            break;
        }

        if (alpha >= beta) {
            return entry.score;
        }
    }

    Score best = INT16_MIN;
    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);
    for (auto &[move, score] : moves) {
        score = scoring_move(move, position);
    }

    for (auto it = moves.begin(); it != moves.end(); ++it) {
        std::iter_swap(it, std::ranges::max_element(it, moves.end(), std::greater{}, &MoveScore::score));

        if (!position.try_do_pseudo(it->move)) {
            continue;
        }

        const Score score = -negamax(position, depth - 1, -beta, -alpha);
        position.undo_move();

        best = std::max(best, score);
        alpha = std::max(alpha, score);
        if (alpha >= beta) {
            return best;
        }
    }

    if (best != INT16_MIN) {
        return best;
    }
    return position.is_in_check() ? INT16_MIN : 0;
}

} // namespace cheslib
