#pragma once

#include "cheslib/array.hpp"
#include "cheslib/move.hpp"

#include "pieces.hpp"
#include "state.hpp"
#include "types.hpp"

namespace cheslib {

class Position {
  public:
    Position(Pieces &&pieces, State state);
    static Position initial();

    const Pieces &pieces() const;
    State state() const;
    ZKey key() const;

    void do_move(Move move);
    void undo_move();

  private:
    struct HistoryEntry {
        ZKey key;
        Move move;
        State state;
        Piece captured;
    };

    template <Side Us>
    void do_move_of(Move move);
    template <Side Us>
    void undo_move_of();

  private:
    Pieces _pieces;
    State _state;
    ZKey _key;
    Array<HistoryEntry, 512> _history;
};

} // namespace cheslib
