#include "history_heuristic.hpp"

namespace cheslib {

Score HistoryHeuristic::get(const Piece piece, const Square to) const {
    assert(piece < PieceCNT);
    assert(to < SquareCNT);

    return _scores[piece][to].load(std::memory_order::acquire);
}

void HistoryHeuristic::update(const Position &position, const Move move, const unsigned depth) {
    const Piece piece = position.pieces().at(move.from());
    const Square to = move.to();

    assert(piece < PieceCNT);
    assert(to < SquareCNT);
    assert(move.flag() == QuietMove);

    std::atomic_int16_t &entry = _scores[piece][to];
    const Score current = entry.load(std::memory_order::acquire);
    const Score next = 16 * depth + current + (current >> 6);
    entry.store(next, std::memory_order::release);
}

void HistoryHeuristic::reset() {
    for (auto &row : _scores) {
        for (std::atomic_int16_t &score : row) {
            score.store(0, std::memory_order::release);
        }
    }
}

} // namespace cheslib
