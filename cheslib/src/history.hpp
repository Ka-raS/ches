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

    template <typename... Args>
        requires std::constructible_from<Undo, Args...>
    constexpr void push(Args &&...args);
    constexpr Undo pop();

  private:
    static constexpr size_t s_size = 512;
    Undo _undos[s_size];
    size_t _size;
};

constexpr HistoryStack::HistoryStack() : _size(0) {
}

constexpr size_t HistoryStack::size() const {
    return _size;
}

template <typename... Args>
    requires std::constructible_from<Undo, Args...>
constexpr void HistoryStack::push(Args &&...args) {
    assert(size() < s_size);
    _undos[_size] = Undo{std::forward<Args>(args)...};
    ++_size;
}

constexpr Undo HistoryStack::pop() {
    assert(_size > 0);
    --_size;
    return _undos[_size];
}

} // namespace cheslib
