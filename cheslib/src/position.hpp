#pragma once

#include "cheslib/array.hpp"
#include "pieces.hpp"
#include "position_state.hpp"

#include <vector>

namespace cheslib {

struct alignas(16) MoveEntry {
    ZobristKey key;
    PositionState state;
    Move move;
    Piece captured;
};

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

    /**
     * `Engine` calls this after each `do_legal`
     * @param buffer stores the trimmed `MoveEntry`
     */
    void trim_history(std::vector<MoveEntry> &buffer);
    void undo_move_restore_history(std::vector<MoveEntry> &buffer);

  private:
    bool is_attacking(Square at, Side us) const;

  private:
    Pieces _pieces;
    PositionState _state;
    ZobristKey _key;
    Array<MoveEntry, 128> _history;
};

} // namespace cheslib
