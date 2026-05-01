#pragma once

#include <array>

#include "cheslib/move.hpp"

namespace cheslib {

class Position;

class Engine {
  public:
    Engine();
    ~Engine();

    const std::array<Piece, SquareCNT> &board() const;

  private:
    alignas(8) std::byte _position_buffer[8400];
    Position &_position;
};

} // namespace cheslib
