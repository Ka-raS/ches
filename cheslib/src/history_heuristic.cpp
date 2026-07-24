#include "history_heuristic.hpp"

namespace cheslib {

Score HistoryHeuristic::get(const Piece piece, const Square to) const {
    assert(piece < PieceCNT);
    assert(to < SquareCNT);

    return _scores[piece][to].load(std::memory_order::acquire);
}

void HistoryHeuristic::reset() {
    for (auto &row : _scores) {
        for (std::atomic_int16_t &score : row) {
            score.store(0, std::memory_order::release);
        }
    }
}

void HistoryHeuristic::update(
    const MoveScore *const front, const MoveScore *const back, const Pieces &pieces, const Score bonus
) {
    constexpr Score max_heuristic = 1 << 14;

    if (back->move.flag() == QuietMove) {
        std::atomic_int16_t &entry = _scores[pieces.at(back->move.from())][back->move.to()];
        const Score current = entry.load(std::memory_order::relaxed);
        entry.store(current + bonus - (current * bonus) / max_heuristic, std::memory_order::relaxed);
    }

    for (const MoveScore *it = front; it != back; ++it) {
        if (it->move != Move::none() && it->move.flag() == QuietMove) {
            std::atomic_int16_t &entry = _scores[pieces.at(it->move.from())][it->move.to()];
            const Score current = entry.load(std::memory_order::relaxed);
            entry.store(current - bonus - (current * bonus) / max_heuristic, std::memory_order::relaxed);
        }
    }
}

} // namespace cheslib
