#pragma once

#include "cheslib/move.hpp"

namespace cheslib {

class MoveList {
  public:
    MoveList();
    const Move *begin() const;
    const Move *end() const;
    const Move *find(Square from, Square to) const; // return nullptr if not found
    bool has(Move move) const;
    void add(Move move);

  private:
    Move _moves[256];
    std::size_t _size;
};

} // namespace cheslib
