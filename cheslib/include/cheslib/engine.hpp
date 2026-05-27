#pragma once

#include <array>

#include "cheslib/array.hpp"
#include "cheslib/move.hpp"

namespace cheslib {

class Engine {
  public:
    /// init to starting position, white ready to play
    /// @param search_depth minimum set to 1
    /// @param thread_count if <= 0 then using `hardware_concurrency + thread_count`
    Engine(unsigned search_depth, int thread_count);
    ~Engine();

    bool is_game_over() const;
    void reset_game();

    const std::array<Piece, SquareCNT> &board() const;
    const Array<Move, 256> &legal_moves() const;

    void do_move(Move move);
    void start_move_search();
    bool is_searching() const;
    Move search_result() const;

  private:
    struct PositionImpl;
    struct NegamaxImpl;
    static constexpr size_t BufferAlign = 8;

  private:
    alignas(BufferAlign) std::byte _buffer[5896];
    PositionImpl &_position;
    NegamaxImpl &_negamax;
    Array<Move, 256> _legal_moves;
};

} // namespace cheslib
