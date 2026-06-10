#include <algorithm>
#include <iostream>

#include "evaluate.hpp"
#include "movegen.hpp"
#include "negamax.hpp"

namespace cheslib {

Negamax::Negamax(const unsigned search_depth, const int thread_count)
    : _transpositions{std::make_unique<TranspositionTable>()},
      _heuristics{},
      _max_depth{std::clamp(search_depth, 1u, 16u)},
      _result{MoveScore{Move::none()}},
      _threads(calculate_thread_count(thread_count)) {}

size_t Negamax::calculate_thread_count(int requested) {
    const int max_count = std::thread::hardware_concurrency();
    if (requested <= 0) {
        requested += max_count;
    }
    return std::clamp(requested, 1, max_count);
}

void Negamax::reset() {
    stop_search();
    wait_while_searching();
    _heuristics.reset();
    _transpositions->reset();
}

void Negamax::wait_while_searching() const {
    for (const Thread &thread : _threads) {
        thread.wait_while_running();
    }
}

Move Negamax::result() const {
    assert(!is_searching());
    return _result.load(std::memory_order_acquire).move;
}

void Negamax::stop_search() {
    _stop.store(true, std::memory_order_release);
}

bool Negamax::is_searching() const {
    for (const Thread &thread : _threads) {
        if (thread.state() == Thread::State::Running) {
            return true;
        }
    }
    return false;
}

Score Negamax::score_move(const Move move, const Position &position) const {
    const Pieces &pieces = position.pieces();
    const Square to = move.to();
    const Piece moved = pieces.at(move.from());

    if (move.is_capture()) {
        Score captured;
        if (move.flag() == EnPassant) {
            captured = evaluate::material(Pawn);
        } else {
            captured = evaluate::material(pieces.at(to));
        }

        return captured - evaluate::material(moved);
    }

    if (move.is_promotion()) {
        const PieceType promoted = move.promoted_piece();
        return evaluate::material(promoted) - evaluate::material(Pawn);
    }

    return _heuristics.get(moved, to);
}

void Negamax::start_search(const Position &position, const Array<Move, 256> &legal_moves) {
    assert(legal_moves.size() > 0);
    if (is_searching()) {
        return;
    }

    auto sort_moves = [this, &position, &legal_moves] {
        Array<MoveScore, 256> moves;
        for (const Move move : legal_moves) {
            moves.push(move, (int16_t)score_move(move, position));
        }

        std::ranges::sort(moves, std::ranges::greater{}, &MoveScore::score);
        return moves;
    };

    const size_t worker_count = std::min(_threads.size(), legal_moves.size());
    std::atomic_size_t pending_assignments = worker_count;

    auto assign_moves = [worker_count, &pending_assignments, sorted_moves = sort_moves()](const size_t worker_index) {
        assert(worker_index < sorted_moves.size());

        std::vector<MoveScore> moves;
        moves.reserve(1 + (sorted_moves.size() - 1 - worker_index) / worker_count);

        for (size_t i = worker_index; i < sorted_moves.size(); i += worker_count) {
            moves.push_back(sorted_moves[i]);
        }

        pending_assignments.fetch_sub(1, std::memory_order_acq_rel);
        assert(!moves.empty());
        return moves;
    };

    _result.store(MoveScore{Move::none(), -INT16_MAX}, std::memory_order_release);
    _stop.store(false, std::memory_order_release);

    for (size_t i = 0; i < worker_count; ++i) {
        // TODO: this 32 bytes lamda causes std::function to heap alloc

        _threads[i].assign_job([this, &position, &assign_moves, i] {
            const MoveScore best = iterative_deepening(Position{position}, assign_moves(i));
            // &position and &assign_moves are now dangling references

            MoveScore result = _result.load(std::memory_order_acquire);

            while ( // compare and swap loop
                best.score >= result.score &&
                !_result.compare_exchange_weak(result, best, std::memory_order_release, std::memory_order_acquire)) {}
        });
    }

    while (pending_assignments.load(std::memory_order_acquire) > 0) {
        std::this_thread::yield();
    }
}

MoveScore Negamax::iterative_deepening(Position position, std::vector<MoveScore> legal_moves) {
    assert(!legal_moves.empty());

    MoveScore best;

    for (unsigned depth = 1; depth < _max_depth; ++depth) {
        // try transposition table move first
        if (const Transposition entry = _transpositions->get(position.key()); //
            entry.is_match(position.key())) {

            const auto it = std::ranges::find(legal_moves, entry.move(), &MoveScore::move);
            if (it != legal_moves.end()) {
                it->score = INT16_MAX;
            }
        }

        std::ranges::sort(legal_moves, std::ranges::greater{}, &MoveScore::score);
        best.score = -INT16_MAX;

        for (MoveScore &current : legal_moves) {
            position.do_legal(current.move);
            current.score = -negamax(position, depth, -INT16_MAX, -best.score);
            position.undo(current.move);

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
    if (depth == 0 || _stop.load(std::memory_order_acquire)) {
        return evaluate::positional(position);
    }

    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);
    for (auto &[move, score] : moves) {
        score = score_move(move, position);
    }

    // try shrink alpha beta
    if (const Transposition entry = _transpositions->get(position.key()); //
        entry.is_match(position.key())) {

        const auto it = std::ranges::find(moves, entry.move(), &MoveScore::move);
        if (it != moves.end() && position.try_do_pseudo(entry.move())) {
            it->score = INT16_MAX;
            position.undo(entry.move());

            if (entry.depth() >= depth) {
                switch (entry.bound()) {
                case Bound::Exact:
                    return entry.score();

                case Bound::Lower:
                    alpha = std::max(alpha, entry.score());
                    break;

                case Bound::Upper:
                    beta = std::min(beta, entry.score());
                    break;
                }

                if (alpha >= beta) {
                    return entry.score();
                }
            }
        }
    }
    // const alpha, beta

    MoveScore best{Move::none(), -INT16_MAX};
    const MoveScore *const moves_end = moves.end();

    for (MoveScore *it = moves.begin(); it != moves_end; ++it) {
        std::iter_swap(it, std::ranges::max_element(it, moves_end, {}, &MoveScore::score));

        const Move move = it->move;
        if (!position.try_do_pseudo(move)) {
            continue;
        }

        const Score score = -negamax(position, depth - 1, -beta, -alpha);
        position.undo(move);

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
        if (position.is_in_check()) {
            return checkmated;
        } else {
            return stalemate;
        }
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
