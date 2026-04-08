#pragma once

#include <utility>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

#include "state_info.hpp"
#include "zobrist.hpp"

namespace ches {

struct UndoInfo {
    ZKey key;
    Move move;
    StateInfo state;
    Piece captured;
};

class HistoryStack {
  public:
    constexpr HistoryStack() : _top(_undos) {
    }

    template <typename... Args>
    constexpr void push(Args &&...args) {
        *_top++ = UndoInfo{std::forward<Args>(args)...};
    }

    constexpr const UndoInfo &pop() {
        assert(_top > _undos);
        return *--_top;
    }

  private:
    UndoInfo _undos[512];
    UndoInfo *_top;
};

} // namespace ches
