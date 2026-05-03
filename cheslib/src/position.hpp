#pragma once

#include "cheslib/array.hpp"
#include "cheslib/move.hpp"

#include "pieces.hpp"
#include "state.hpp"
#include "types.hpp"
#include "zobrist.hpp"

namespace cheslib {

class Position {
  public:
    Position(Pieces &&pieces, State state);
    static Position initial();

    const Pieces &pieces() const;
    State state() const;
    ZobristKey key() const;

    void undo_move();
    [[nodiscard]] bool try_do_pseudo(Move move);

  private:
    struct HistoryEntry {
        ZobristKey key;
        Move move;
        State state;
        Piece captured;
    };

    bool is_attacked(Square at, Side us) const;
    void do_pseudo(Move move, Side us, Square from, Square to, MoveFlag move_flag);

  private:
    Pieces _pieces;
    State _state;
    ZobristKey _key;
    Array<HistoryEntry, 512> _history;
};

} // namespace cheslib
