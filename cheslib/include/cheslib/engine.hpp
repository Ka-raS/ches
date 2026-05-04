#pragma once

#include <array>

#include "cheslib/move_list.hpp"

namespace cheslib {

class Engine {
  public:
    Engine();
    ~Engine();

    const std::array<Piece, SquareCNT> &board() const;

    [[nodiscard]] bool try_do_move(Move move);
    MoveList generate_legal_moves() const;

  private:
    struct PositionImpl;
    static constexpr std::size_t PositionSize = 8400;
    static constexpr std::size_t PositionAlign = 8;

    static PositionImpl &construct_position(std::byte *buffer);

  private:
    alignas(PositionAlign) std::byte _position_buffer[PositionSize];
    PositionImpl &_position;
};

} // namespace cheslib
