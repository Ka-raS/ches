#pragma once

#include "cheslib/array.hpp"
#include "pieces.hpp"
#include "position_state.hpp"

namespace cheslib {

class Position {
  public:
    Position(PositionState state, const std::array<Piece, SquareCNT> &board);
    static Position initial();

    const Pieces &pieces() const;
    PositionState state() const;
    ZobristKey key() const;

    bool is_in_check() const;
    bool is_50move_draw() const;
    bool is_3fold_repetition() const;
    bool is_insufficient_material() const;

    [[nodiscard]] bool try_do_pseudo(Move move); ///< @return `false` if pseudo move fails king safety
    void do_legal(Move move);
    void undo_move();
    void trim_history(); ///< `Engine` calls this after each `do_legal`

  private:
    bool is_attacking(Square at, Side us) const;

    struct alignas(16) HistoryEntry {
        ZobristKey key;
        PositionState state;
        Move move;
        Piece captured;
    };

  private:
    Pieces _pieces;
    PositionState _state;
    ZobristKey _key;
    Array<HistoryEntry, 128> _history;
};

} // namespace cheslib
