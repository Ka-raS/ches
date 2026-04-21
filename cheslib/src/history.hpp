#pragma once

#include <utility>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

#include "state.hpp"
#include "zobrist.hpp"

namespace cheslib {

struct Undo {
    ZKey key;
    Move move;
    State state;
    Piece captured;
};

class HistoryStack {
  public:
    constexpr HistoryStack();
    constexpr size_t size() const;
    constexpr void push(ZKey key, Move move, State state, Piece captured);
    constexpr Undo pop();

  private:
    static constexpr size_t s_max_size = 512;
    Undo _undos[s_max_size];
    size_t _size;
};

constexpr HistoryStack::HistoryStack() : _undos{}, _size(0) {
}

constexpr size_t HistoryStack::size() const {
    return _size;
}

constexpr void HistoryStack::push(ZKey key, Move move, State state, Piece captured) {
    assert(_size < s_max_size);
    _undos[_size] = Undo{key, move, state, captured};
    ++_size;
}

constexpr Undo HistoryStack::pop() {
    assert(_size > 0);
    --_size;
    return std::move(_undos[_size]);
}

} // namespace cheslib
