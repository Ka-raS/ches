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

    /// @return false if pseudo move fails king safety
    [[nodiscard]] bool try_do_pseudo(Move move);
    void do_legal(Move move);
    void undo(Move move);
    void trim_history();

  private:
    bool is_attacking(Square at, Side us) const;

    struct HistoryEntry {
        ZobristKey key;
        PositionState state;
        Piece captured;
    };

  private:
    Pieces _pieces;
    PositionState _state;
    ZobristKey _key;
    Array<HistoryEntry, 128> _history;
};

} // namespace cheslib
