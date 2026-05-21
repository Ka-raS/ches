#pragma once

#include "cheslib/array.hpp"

#include "pieces.hpp"
#include "position_state.hpp"

namespace cheslib {

class Position {
  public:
    Position(Pieces &&pieces, PositionState state);
    static Position initial();

    const Pieces &pieces() const;
    PositionState state() const;
    ZobristKey key() const;

    bool is_in_check() const;
    bool is_50move_draw() const;
    bool is_3fold_repetition() const;

    /// @return false if pseudo move fails king safety
    [[nodiscard]] bool try_do_pseudo(Move move);
    void do_move(Move move);
    void undo_move();
    void trim_history();

  private:
    bool is_attacking(Square at, Side us) const;

    struct HistoryEntry {
        ZobristKey key;
        Move move;
        PositionState state;
        Piece captured;
    };

  private:
    Pieces _pieces;
    PositionState _state;
    ZobristKey _key;
    Array<HistoryEntry, 256> _history;
};

} // namespace cheslib
