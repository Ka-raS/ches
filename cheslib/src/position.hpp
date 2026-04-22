#pragma once

#include <array>
#include <cstdint>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

#include "history.hpp"
#include "pieces.hpp"
#include "state.hpp"
#include "zobrist.hpp"

namespace cheslib {

class Position {
  public:
    constexpr Position() = default;
    inline Position(Pieces &&pieces, State state);
    static inline Position initial();

    constexpr const Pieces &pieces() const;
    constexpr State state() const;
    constexpr ZKey key() const;

    void do_move(Move move);
    void undo_move();

  private:
    template <Side Us>
    void do_move_of(Move move);
    template <Side Us>
    void undo_move_of();

  private:
    Pieces _pieces;
    State _state;
    ZKey _key;
    HistoryStack _history;
};

inline Position::Position(Pieces &&pieces, State state)
    : _pieces(std::move(pieces)), _state(state), _key(zobrist::hash(_pieces.board(), _state)), _history() {
}

inline Position Position::initial() {
    return Position(Pieces::initial(), State::initial());
}

constexpr const Pieces &Position::pieces() const {
    return _pieces;
}

constexpr State Position::state() const {
    return _state;
}

constexpr ZKey Position::key() const {
    return _key;
}

} // namespace cheslib
