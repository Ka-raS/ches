#pragma once

#include "cheslib/array.hpp"
#include "cheslib/move.hpp"

#include <array>

namespace cheslib {

enum class ChessState : unsigned {
    OnGoing,
    WhiteWin,
    BlackWin,
    Stalemate,
    Draw50Move,
    Draw3Repetition,
    DrawInsufficientMaterial
};

class Engine {
  public:
    /**
     * init to starting position, white ready to play
     * @param search_depth clamped to `[1, 16]`
     * @param thread_count clamped to `[1, hardware_concurrency]`, if `<= 0` use `hardware_concurrency + thread_count`
     */
    Engine(unsigned search_depth, int thread_count);

    ChessState state() const;
    const Array<Move, 256> &legal_moves() const;
    const std::array<Piece, SquareCNT> &board() const;

    /// reset to starting position, white ready to play
    void reset_game();

    /**
     * @throw `std::logic_error` if `state() != ChessState::OnGoing`
     * @throw `std::invalid_argument` if `move` not in `legal_moves()`
     */
    [[nodiscard]] ChessState do_move(Move move);

    /**
     * non blocking, do nothing if `is_searching()`
     * @throw `std::logic_error` if `state() != ChessState::OnGoing`
     */
    void start_move_search();

    /**
     * non blocking, do nothing if `!is_searching()`
     * @note `is_searching()` won't be `false` right after this call
     */
    void stop_move_search();

    /// blocking until `!is_searching()`
    void wait_while_searching() const;

    bool is_searching() const;

    /**
     * @note result is cached until next `start_move_search()`
     * @return the best move found if `!is_searching()`,
     * else the current best move found so far (can be `Move::none()` if called too early)
     */
    Move search_result() const;

    ~Engine();
    Engine(Engine &&) = delete;
    Engine(const Engine &) = delete;
    Engine &operator=(Engine &&) = delete;
    Engine &operator=(const Engine &) = delete;

  private:
    struct Impl;
    Impl *pimpl();
    const Impl *pimpl() const;

  private:
    static constexpr size_t BufferAlign = 8;

    alignas(BufferAlign) std::byte _buffer[3848];
    Array<Move, 256> _legal_moves;
};

} // namespace cheslib
