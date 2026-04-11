#pragma once

#include <utility>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

#include "state.hpp"
#include "zobrist.hpp"

namespace ches {

struct Undo {
    ZKey key;
    Move move;
    State state;
    Piece captured;
};

class HistoryStack {
  public:
    constexpr HistoryStack() : _top(_undos) {
    }

    constexpr bool empty() const {
        return _top == _undos;
    }

    constexpr size_t size() const {
        return _top - _undos;
    }

    template <typename... Args>
    constexpr void push(Args &&...args) {
        assert(size() < Size);
        *_top++ = Undo{std::forward<Args>(args)...};
    }

    constexpr const Undo &pop() {
        assert(_top > _undos);
        return *--_top;
    }

  private:
    static constexpr size_t Size = 512;
    Undo _undos[Size];
    Undo *_top;
};

} // namespace ches
