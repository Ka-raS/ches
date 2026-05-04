#pragma once

#include "history_stack.hpp"
#include "pieces.hpp"
#include "state.hpp"

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
    bool is_attacked(Square at, Side us) const;
    void do_pseudo(Move move, Side us, Square from, Square to, MoveFlag move_flag);

  private:
    Pieces _pieces;
    State _state;
    ZobristKey _key;
    HistoryStack _history;
};

} // namespace cheslib
