#pragma once

#include <array>
#include <bit>
#include <cstdint>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

#include "history_stack.hpp"
#include "piece_bitboards.hpp"
#include "state.hpp"
#include "zobrist.hpp"

namespace ches {

class Position {
  public:
    constexpr Position() = default;

    Position(const PieceBitboards &pieces, State state)
        : _pieces(pieces), _state(state), _board(init_board(pieces)), _key(zobrist_hash(_board, _state)) {
    }

    static Position initial() {
        return Position(PieceBitboards::initial(), State::initial());
    }

    void do_move(Move move);

    void undo_move();

    constexpr const PieceBitboards &pieces() const {
        return _pieces;
    }

    constexpr State state() const {
        return _state;
    }

    constexpr const std::array<Piece, SquareCNT> &board() const {
        return _board;
    }

    constexpr int count(Piece piece) const {
        return std::popcount(_pieces.get(piece));
    }

  private:
    static std::array<Piece, SquareCNT> init_board(const PieceBitboards &pieces);

    inline void set_piece_no_hash(Square sq, Piece piece) {
        assert(sq < SquareCNT);
        assert(piece < PieceCNT);

        _pieces.set(sq, piece);
        _board[sq] = piece;
    }

    inline void unset_piece_no_hash(Square sq) {
        assert(sq < SquareCNT);
        assert(_board[sq] < PieceCNT);

        _pieces.unset(sq, _board[sq]);
        _board[sq] = PieceCNT;
    }

    inline void set_piece(Square sq, Piece piece) {
        _key ^= zobrist_piece(piece, sq);
        set_piece_no_hash(sq, piece);
    }

    inline void unset_piece(Square sq) {
        _key ^= zobrist_piece(_board[sq], sq);
        unset_piece_no_hash(sq);
    }

  private:
    PieceBitboards _pieces;
    std::array<Piece, SquareCNT> _board;
    State _state;
    ZKey _key;
    HistoryStack _history;
};

} // namespace ches
