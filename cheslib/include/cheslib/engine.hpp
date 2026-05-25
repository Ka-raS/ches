#pragma once

#include <array>

#include "cheslib/array.hpp"
#include "cheslib/move.hpp"

namespace cheslib {

class Engine {
  public:
    /// init to starting position, white ready to play
    /// @param thread_count if negative then using `hardware_concurrency + thread_count`
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
    static size_t calculate_thread_count(int count);

    struct PositionImpl;
    static constexpr size_t PositionSize = 4304;
    static constexpr size_t PositionAlign = 8;

    struct NegamaxImpl;
    static constexpr size_t NegamaxSize = 1592;
    static constexpr size_t NegamaxAlign = 8;

  private:
    alignas(PositionAlign) std::byte _position_buffer[PositionSize];
    alignas(NegamaxAlign) std::byte _negamax_buffer[NegamaxSize];
    PositionImpl &_position;
    NegamaxImpl &_negamax;
    Array<Move, 256> _legal_moves;
};

} // namespace cheslib
