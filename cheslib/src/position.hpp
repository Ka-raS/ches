#pragma once

#include "move_history.hpp"
#include "pieces.hpp"

namespace cheslib {

class Position {
  public:
    Position(Pieces &&pieces, State state);
    static Position initial();

    const Pieces &pieces() const;
    State state() const;
    ZobristKey key() const;

    bool is_in_check() const;
    bool is_drawn() const;

    /// @return false if pseudo move fails king safety
    [[nodiscard]] bool try_do_pseudo(Move move);
    void do_move(Move move);
    void undo_move();

  private:
    bool is_attacking(Square at, Side us) const;

  private:
    Pieces _pieces;
    State _state;
    ZobristKey _key;
    MoveHistory _history;
};

} // namespace cheslib
