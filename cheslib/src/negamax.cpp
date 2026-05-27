#include <algorithm>
#include <iostream>

#include "eval.hpp"
#include "movegen.hpp"
#include "negamax.hpp"

namespace cheslib {

Negamax::Negamax(unsigned search_depth, int threads_requested)
    : _max_depth(std::max(1u, search_depth)),
      _heuristics(),
      _transpositions(std::make_unique<TranspositionTable>()),
      _threads(calculate_thread_count(threads_requested)) {}

size_t Negamax::calculate_thread_count(int requested) {
    const int max_count = std::thread::hardware_concurrency();
    if (requested <= 0) {
        requested += max_count;
    }
    return std::clamp(requested, 1, max_count);
}

void Negamax::reset() {
    _heuristics.reset();
    _transpositions->reset();
}

Move Negamax::result() const {
    assert(!is_searching());
    MoveScore result = _result.load(std::memory_order_acquire);
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

Score Negamax::score_move(Move move, const Position &position) const {
    const Pieces &pieces = position.pieces();
    const Square to = move.to();
    const Piece moved = pieces.at(move.from());

    if (move.is_capture()) {
        Score captured;
        if (move.flag() == EnPassant) {
            captured = eval::value_of(Pawn);
        } else {
            captured = eval::value_of(pieces.at(to));
        }

        return captured - eval::value_of(moved);
    }

    if (move.is_promotion()) {
        PieceType promoted = move.promoted_piece();
        return eval::value_of(promoted) - eval::value_of(Pawn);
    }

    return _heuristics.get(moved, to);
}

void Negamax::start_search(const Position &position, const Array<Move, 256> &legal_moves) {
    assert(legal_moves.size() > 0);
    const size_t thread_count = _threads.size();

    auto sort_moves = [this, &position, &legal_moves]() -> Array<MoveScore, 256> {
        Array<MoveScore, 256> moves;
        for (Move move : legal_moves) {
            moves.push(move, (int16_t)score_move(move, position));
        }

        std::ranges::sort(moves, std::ranges::greater{}, &MoveScore::score);
        return moves;
    };

    auto assign_moves = [thread_count, sorted = sort_moves()](size_t thread_index) -> std::vector<MoveScore> {
        assert(thread_index < sorted.size());

        std::vector<MoveScore> moves;
        moves.reserve(1 + (sorted.size() - 1 - thread_index) / thread_count);

        for (size_t i = thread_index; i < sorted.size(); i += thread_count) {
            moves.push_back(sorted[i]);
        }

        assert(!moves.empty());
        return moves;
    };

    _result.store({.score = -INT16_MAX}, std::memory_order_release);

    for (size_t i = 0; i < thread_count; ++i) {
        if (i >= legal_moves.size()) {
            break;
        }

        _threads[i].assign_job([this, pos = position, assigned = assign_moves(i)]() mutable -> void {
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

    MoveScore best;

    for (unsigned depth = 1; depth < _max_depth; ++depth) {
        // try transposition table move first
        if (const Transposition entry = _transpositions->get(position.key()); //
            entry.is_match(position.key())) {

            auto found = std::ranges::find(legal_moves, entry.move(), &MoveScore::move);
            if (found != legal_moves.end()) {
                found->score = INT16_MAX;
            }
        }

        std::ranges::sort(legal_moves, std::ranges::greater{}, &MoveScore::score);
        best.score = -INT16_MAX;

        for (MoveScore &current : legal_moves) {
            position.do_move(current.move);
            current.score = -negamax(position, depth, -INT16_MAX, -best.score);
            position.undo_move();

            if (best.score < current.score) {
                best = current;
            }
        }
    }

    assert(best.score > -INT16_MAX);
    return best;
}

Score Negamax::negamax(Position &position, const uint8_t depth, Score alpha, Score beta) {
    if (position.is_50move_draw() || position.is_3fold_repetition()) {
        return 0;
    }
    if (depth == 0) {
        return eval::evaluate(position);
    }

    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);
    for (auto &[move, score] : moves) {
        score = score_move(move, position);
    }

    // try shrink alpha beta
    if (const Transposition entry = _transpositions->get(position.key()); //
        entry.is_match(position.key())) {

        auto found = std::ranges::find(moves, entry.move(), &MoveScore::move);
        if (found != moves.end() && position.try_do_pseudo(entry.move())) {
            found->score = INT16_MAX;
            position.undo_move();

            if (entry.depth() >= depth) {
                const Score entry_score = entry.score();

                switch (entry.bound()) {
                case Bound::Exact:
                    return entry_score;

                case Bound::Lower:
                    alpha = std::max(alpha, entry_score);
                    break;

                case Bound::Upper:
                    beta = std::min(beta, entry_score);
                    break;
                }

                if (alpha >= beta) {
                    return entry_score;
                }
            }
        }
    }
    // const alpha, beta

    MoveScore best = {.move = Move::none(), .score = -INT16_MAX};

    for (MoveScore *it = moves.begin(); it != moves.end(); ++it) {
        std::iter_swap(it, std::ranges::max_element(it, moves.end(), {}, &MoveScore::score));

        const Move move = it->move;
        if (!position.try_do_pseudo(move)) {
            continue;
        }

        const Score score = -negamax(position, depth - 1, -beta, -alpha);
        position.undo_move();

        if (best.score < score) {
            best.score = score;
            best.move = move;

            if (score >= beta) {
                _heuristics.update(position, move, depth);
                break;
            }
        }
    }
    // const best

    if (best.move == Move::none()) {
        constexpr Score checkmated = -INT16_MAX;
        constexpr Score stalemate = 0;
        return position.is_in_check() ? checkmated : stalemate;
    }

    { // update transposition table
        Bound bound;
        if (best.score <= alpha) {
            bound = Bound::Upper;
        } else if (best.score >= beta) {
            bound = Bound::Lower;
        } else {
            bound = Bound::Exact;
        }
        _transpositions->store(position.key(), best, bound, depth);
    }

    return best.score;
}

} // namespace cheslib
