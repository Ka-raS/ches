#pragma once

#include "cheslib/move.hpp"

namespace cheslib {

class MoveList {
  public:
    static constexpr std::size_t MaxSize = 256;

  public:
    MoveList();
    std::size_t size() const;
    bool has(Move move) const;
    const Move *begin() const;
    const Move *end() const;

    /// @return `nullptr` if not found
    const Move *find(Square from, Square to) const;

    /// preconditions: `size() < MaxSize`
    void add(Move move);

  private:
    Move _moves[MaxSize];
    std::size_t _size;
};

} // namespace cheslib
