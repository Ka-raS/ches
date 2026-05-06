#pragma once

#include "cheslib/move.hpp"

namespace cheslib {

class MoveList {
  public:
    static constexpr size_t MaxSize = 256;

  public:
    MoveList();
    size_t size() const;
    bool has(Move move) const;
    const Move *begin() const;
    const Move *end() const;

    /// @return `nullptr` if not found
    const Move *find(Square from, Square to) const;

    /// preconditions: `size() < MaxSize`
    void add(Move move);

  private:
    Move _moves[MaxSize];
    size_t _size;
};

} // namespace cheslib
