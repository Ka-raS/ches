#include "history_heuristic.hpp"

namespace cheslib {

HistoryHeuristic::HistoryHeuristic()
    : _entries{} {};

Score HistoryHeuristic::get(Piece piece, Square to) const {
    assert(piece < PieceCNT);
    assert(to < SquareCNT);

    return _entries[piece][to].load(std::memory_order_relaxed);
}

void HistoryHeuristic::update(const Position &position, Move move, uint8_t depth) {
    if (!move.flag() == QuietMove) {
        return;
    }

    const Piece piece = position.pieces().at(move.from());
    const Square to = move.to();

    assert(piece < PieceCNT);
    assert(to < SquareCNT);

    std::atomic_int16_t &entry = _entries[piece][to];
    Score current = entry.load(std::memory_order_relaxed);
    Score next = 16 * depth + current + (current >> 6);
    entry.store(next, std::memory_order_relaxed);
}

void HistoryHeuristic::reset() {
    for (auto &row : _entries) {
        for (std::atomic_int16_t &entry : row) {
            entry.store(0, std::memory_order_relaxed);
        }
    }
}

} // namespace cheslib
