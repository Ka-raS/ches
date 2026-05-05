#pragma once

#include "cheslib/move.hpp"

#include "pieces.hpp"
#include "state.hpp"
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
    struct MoveEntry {
        ZobristKey key;
        Move move;
        State state;
        Piece captured;
    };

  private:
    bool is_attacked(Square at, Side us) const;
    void do_pseudo(Move move, Side us, Square from, Square to, MoveFlag move_flag);
    void push_history(MoveEntry entry);
    MoveEntry pop_history();

  private:
    Pieces _pieces;
    State _state;
    ZobristKey _key;

    MoveEntry _history[512];
    std::size_t _history_size;
};

} // namespace cheslib
