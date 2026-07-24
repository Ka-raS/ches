#include "negamax.hpp"
#include "evaluate.hpp"
#include "movegen.hpp"

#include <algorithm>

namespace cheslib {

namespace {

// clang-format off
constexpr uint16_t VictimAggressor[PieceTypeCNT - 1][PieceTypeCNT] = {
    //           Pawn Knight Bishop   Rook  Queen   King
    /*Pawn  */ {17500, 17400, 17300, 17200, 17100, 17000},
    /*Knight*/ {18500, 18400, 18300, 18200, 18100, 18000},
    /*Bishop*/ {19500, 19400, 19300, 19200, 19100, 19000},
    /*Rook  */ {20500, 20400, 20300, 20200, 20100, 20000},
    /*Queen */ {21500, 21400, 21300, 21200, 21100,     0}
};
// clang-format on

size_t calculate_thread_count(int requested) {
    const int max_count = std::thread::hardware_concurrency();
    if (requested <= 0) {
        requested += max_count;
    }
    return std::clamp(requested, 1, max_count);
}

} // namespace

Negamax::Negamax(const unsigned search_depth, const int thread_count) :
    _transpositions{std::make_unique<TranspositionTable>()},
    _heuristics{},
    _result{MoveScore{Move::none()}},
    _max_depth(std::clamp(search_depth, 2u, 15u)),
    _threads(calculate_thread_count(thread_count)) {}

void Negamax::reset() {
    stop_search();
    wait_while_searching();
    _transpositions->reset();
    _heuristics.reset();
}

void Negamax::wait_while_searching() const {
    for (const Thread &thread : _threads) {
        thread.state().wait(Thread::State::Running, std::memory_order::acquire);
    }
}

Move Negamax::result() const {
    assert(!is_searching());
    return _result.load(std::memory_order::acquire).move;
}

void Negamax::stop_search() {
    _stop.store(true, std::memory_order::release);
}

bool Negamax::is_searching() const {
    for (const Thread &thread : _threads) {
        if (thread.state().load(std::memory_order::acquire) == Thread::State::Running) {
            return true;
        }
    }
    return false;
}

int16_t Negamax::scoring(const Move move, const Pieces &pieces) const {
    const Square to = move.to();
    const Piece moved = pieces.at(move.from());

    if (move.is_capture()) {
        return (move.flag() == EnPassant) ? VictimAggressor[Pawn][Pawn]
                                          : VictimAggressor[type_of(pieces.at(to))][type_of(moved)];
    }
    if (move.is_promotion()) {
        return VictimAggressor[move.promoted_piece()][Pawn];
    }

    return _heuristics.get(moved, to);
}

void Negamax::start_search(const Position &position, const Array<Move, 256> &legal_moves) {
    assert(!is_searching());
    assert(legal_moves.size() > 0);

    std::atomic_size_t next_worker = 0;
    std::atomic_size_t assigned_count = 0;
    const size_t worker_count = std::min(_threads.size(), legal_moves.size());

    auto assign_root_node = [&]() -> RootNode {
        const size_t worker_index = next_worker.fetch_add(1, std::memory_order::relaxed);
        assert(worker_index < worker_count);

        RootNode root{.position = position};
        Array<MoveScore, 256> &moves = root.legal_moves;
        for (size_t i = worker_index; i < legal_moves.size(); i += worker_count) {
            moves.emplace_back(legal_moves[i], scoring(legal_moves[i], position.pieces()));
        }
        assert(moves.size() > 0);

        assigned_count.fetch_add(1, std::memory_order::release);
        // assign_root_node object is now out of scope
        return root;
    };

    _result.store(MoveScore{Move::none(), INT16_MIN}, std::memory_order::release);
    _stop.store(false, std::memory_order::release);

    for (size_t i = 0; i < worker_count; ++i) {
        // std::function wont heap alloc this 16 bytes lambda, neat
        _threads[i].assign_job([this, &assign_root_node]() -> void {
            const MoveScore best = iterative_deepening(assign_root_node());
            // &assign_root_node is now a dangling reference

            MoveScore result = _result.load(std::memory_order::acquire);
            while (
                result.score < best.score &&
                !_result.compare_exchange_weak(result, best, std::memory_order::release, std::memory_order::acquire)) {}
        });
    }

    while (assigned_count.load(std::memory_order::acquire) < worker_count) {
        std::this_thread::yield();
    }
}

MoveScore Negamax::iterative_deepening(RootNode root) {
    MoveScore best;
    auto &[position, legal_moves] = root;

    for (unsigned depth = 2; depth <= _max_depth; ++depth) {
        // try transposition table move first
        if (const Transposition entry = _transpositions->get(position.key()); //
            entry.is_match(position.key())) {

            MoveScore *const it = std::ranges::find(legal_moves, entry.move(), &MoveScore::move);
            if (it != legal_moves.end()) {
                it->score = INT16_MAX;
            }
        }

        std::ranges::sort(legal_moves, std::ranges::greater{}, &MoveScore::score);
        best.score = -INT16_MAX;

        for (MoveScore &current : legal_moves) {
            position.do_legal(current.move);
            current.score = -negamax(position, depth - 1, -INT16_MAX, -best.score);
            position.undo_move();

            if (best.score < current.score) {
                best = current;
            }
        }
    }

    return best;
}

Score Negamax::negamax(Position &position, const unsigned depth, Score alpha, Score beta) {
    if (position.is_50move_draw() || position.is_3fold_repetition() || _stop.load(std::memory_order::acquire)) {
        return 0;
    }
    if (depth == 0) {
        return evaluate::pesto(position);
    }

    const Score original_alpha = alpha;
    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);
    for (auto &[move, score] : moves) {
        score = scoring(move, position.pieces());
    }

    // try to shrink alpha beta
    if (const Transposition entry = _transpositions->get(position.key()); //
        entry.is_match(position.key())) {
        MoveScore *const it = std::ranges::find(moves, entry.move(), &MoveScore::move);

        if (it != moves.end() && (it->score = INT16_MAX) && entry.depth() >= depth &&
            position.try_do_pseudo(entry.move())) {
            position.undo_move();

            switch (entry.bound()) {
            case BoundExact:
                return entry.score();

            case BoundLower:
                alpha = std::max(alpha, entry.score());
                break;

            case BoundUpper:
                beta = std::min(beta, entry.score());
                break;
            }

            if (alpha >= beta) {
                return entry.score();
            }
        }
    }
    // const beta

    Move best_move;
    Score best_score = INT16_MIN;

    for (MoveScore *it = moves.begin(); it != moves.end(); ++it) {
        std::iter_swap(it, std::ranges::max_element(it, moves.end(), {}, &MoveScore::score));

        const Move move = it->move;
        if (!position.try_do_pseudo(move)) {
            it->move = Move::none();
            continue;
        }

        const Score score = -negamax(position, depth - 1, -beta, -alpha);
        position.undo_move();

        if (score >= beta) {
            _transpositions->store(position.key(), move, score, BoundLower, depth);
            _heuristics.update(moves.begin(), it, position.pieces(), 300 * depth - 250);
            return score;
        }

        alpha = std::max(alpha, score);
        if (best_score < score) {
            best_score = score;
            best_move = move;
        }
    }
    // const best

    if (best_score == INT16_MIN) {
        constexpr Score checkmated = 1 - INT16_MAX;
        constexpr Score stalemate = 0;
        return position.is_in_check() ? checkmated : stalemate;
    }

    assert(best_score < beta);
    const Bound bound = (best_score <= original_alpha) ? BoundUpper : BoundExact;
    _transpositions->store(position.key(), best_move, best_score, bound, depth);

    return best_score;
}

} // namespace cheslib
