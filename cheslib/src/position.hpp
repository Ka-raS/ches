#pragma once

#include <array>
#include <cstdint>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

#include "history_stack.hpp"
#include "piece_bitboards.hpp"
#include "state_info.hpp"
#include "zobrist.hpp"

namespace ches {

class Position {
  public:
    constexpr Position() = default;

    constexpr Position(const PieceBitboards &pieces, StateInfo state) : _pieces(pieces), _state(state) {
        _board.fill(PieceCNT);

        for (Piece piece = WhitePawn; piece < PieceCNT; ++piece) {
            Bitboard bb = pieces.get(piece);
            while (bb) {
                Square sq = pop_lsb(bb);
                _board[sq] = piece;
            }
        }
    }

    static constexpr Position initial() {
        return Position(PieceBitboards::initial(), StateInfo::initial());
    }

    void do_move(Move move);

    void undo_move();

    constexpr const PieceBitboards &pieces() const {
        return _pieces;
    }

    constexpr StateInfo state() const {
        return _state;
    }

    constexpr const std::array<Piece, SquareCNT> &board() const {
        return _board;
    }

  private:
    template <Side Us>
    void undo_moved_piece(Square from, Square to, bool is_promotion);

    template <Side Us>
    void undo_captured_piece(Square to, Piece captured, bool is_en_passant);

    template <Side Us>
    void undo_castling(MoveFlag flag);

  private:
    static constexpr Square _rook_initial[2][2] = {{SquareA1, SquareH1}, {SquareA8, SquareH8}};
    static constexpr Square _rook_castled[2][2] = {{SquareD1, SquareF1}, {SquareD8, SquareF8}};

    PieceBitboards _pieces;
    std::array<Piece, SquareCNT> _board;
    StateInfo _state;
    ZKey _key;
    HistoryStack _history;
};

} // namespace ches
