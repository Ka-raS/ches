#pragma once

#include "cheslib/array.hpp"
#include "cheslib/move.hpp"

#include <array>

namespace cheslib {

enum ChessStatus : uint8_t {
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
     * @param search_depth clamped to `[2, 15]`
     * @param thread_count clamped to `[1, min(255, hardware_concurrency)]`, default `0` to `hardware_concurrency / 2`
     * @param transposition_table_kib size in KiB, clamped to `[1 KiB, 4 GiB]`, rounded down to nearest power of 2
     */
    Engine(unsigned search_depth, unsigned thread_count, unsigned transposition_table_kib);

    ChessStatus status() const;
    unsigned search_depth() const;
    unsigned thread_count() const;
    Side side_to_move() const;
    const Array<Move, 256> &legal_moves() const;
    const std::array<Piece, SquareCNT> &board() const;

    /// reset to starting position, white ready to play
    void reset_game();

    /**
     * @param search_depth clamped to `[2, 15]`
     * @throw `std::logic_error` if `is_searching()`
     */
    void set_search_depth(unsigned search_depth);

    /**
     * @param thread_count clamped to `[1, min(255, hardware_concurrency)]`, default `0` to `hardware_concurrency / 2`
     * @throw `std::logic_error` if `is_searching()`
     */
    void set_thread_count(unsigned thread_count);

    /**
     * @throw `std::logic_error` if game over
     * @throw `std::invalid_argument` if `move` not in `legal_moves()`
     */
    [[nodiscard]] ChessStatus do_move(Move move);

    // do nothing if in starting position, any move searching is discarded
    void undo_move();

    /// @throw `std::logic_error` if `is_searching()` or game over
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
    static constexpr size_t BufferAlign = 16;

    alignas(BufferAlign) std::byte _buffer[3880];
    Array<Move, 256> _legal_moves;
};

} // namespace cheslib
