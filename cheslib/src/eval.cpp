#include "eval.hpp"

namespace cheslib::eval {

Score evaluate(const Position &position) {
    int score = 0;
    for (Piece piece : position.pieces().board()) {
        if (piece >= PieceCNT) {
            continue;
        }
        switch (piece) {
        case WhitePawn:
            score += 100;
            break;
        case BlackPawn:
            score -= 100;
            break;
        case WhiteKnight:
            score += 320;
            break;
        case BlackKnight:
            score -= 320;
            break;
        case WhiteBishop:
            score += 330;
            break;
        case BlackBishop:
            score -= 330;
            break;
        case WhiteRook:
            score += 500;
            break;
        case BlackRook:
            score -= 500;
            break;
        case WhiteQueen:
            score += 900;
            break;
        case BlackQueen:
            score -= 900;
            break;

        default:
            break;
        }
    }
    Side us = position.state().side_to_move();
    return score;
}

} // namespace cheslib::eval
