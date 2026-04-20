#pragma once

#include "movegen.hpp"
#include "position.hpp"

namespace cheslib::negamax {

int negamax(Position &position, int depth, int alpha, int beta) {
    if (depth == 0) {
        // return evaluate(position);
        return 0;
    }

    int best = INT32_MIN;

    for (Move move : movegen::pseudo_legals(position.pieces(), position.state())) {
        position.do_move(move);
        const int score = -negamax(position, depth - 1, -beta, -alpha);
        position.undo_move();

        if (best < score) {
            if (beta <= score) {
                return score;
            }
            if (alpha < score) {
                alpha = score;
            }
            best = score;
        }
    }

    return best;
}

Move search_best_move(Position &position, int depth) {
    Move best;
    int alpha = INT32_MIN;
    constexpr int beta = INT32_MAX;
    const MoveList moves = movegen::pseudo_legals(position.pieces(), position.state());

    for (Move move : moves) {
        position.do_move(move);
        const int score = -negamax(position, depth - 1, -beta, -alpha);
        position.undo_move();

        if (alpha < score) {
            alpha = score;
            best = move;
        }
    }

    return best;
}

} // namespace cheslib::negamax
