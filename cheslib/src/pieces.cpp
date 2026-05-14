#include "pieces.hpp"

namespace cheslib {

Pieces::Pieces(std::array<Piece, SquareCNT> &&board)
    : _board(std::move(board)),
      _bitboards{0},
      _side{0},
      _all(0) {
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        Piece piece = _board[sq];
        if (piece >= PieceCNT) {
            continue;
        }

        Side side = types::side_of(piece);
        types::set_square(_all, sq);
        types::set_square(_side[side], sq);
        types::set_square(_bitboards[piece], sq);
    }
}

Pieces Pieces::initial() {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);

    board[SquareE1] = WhiteKing;
    board[SquareE8] = BlackKing;
    board[SquareD1] = WhiteQueen;
    board[SquareD8] = BlackQueen;

    board[SquareA1] = board[SquareH1] = WhiteRook;
    board[SquareA8] = board[SquareH8] = BlackRook;
    board[SquareC1] = board[SquareF1] = WhiteBishop;
    board[SquareC8] = board[SquareF8] = BlackBishop;
    board[SquareB1] = board[SquareG1] = WhiteKnight;
    board[SquareB8] = board[SquareG8] = BlackKnight;

    for (Square sq = SquareA2; sq <= SquareH2; ++sq) {
        board[sq] = WhitePawn;
    }
    for (Square sq = SquareA7; sq <= SquareH7; ++sq) {
        board[sq] = BlackPawn;
    }

    return board;
}

const std::array<Piece, SquareCNT> &Pieces::board() const {
    return _board;
}

Piece Pieces::at(Square sq) const {
    assert(sq < SquareCNT);
    return _board[sq];
}

int Pieces::count(Piece piece) const {
    assert(piece < PieceCNT);
    return std::popcount(_bitboards[piece]);
}

Square Pieces::king_of(Side us) const {
    Piece king = types::piece_of(us, King);
    Bitboard king_bb = _bitboards[king];
    assert(king_bb != 0);
    return (Square)std::countr_zero(king_bb);
}

Bitboard Pieces::all() const {
    return _all;
}

Bitboard Pieces::all_of(Side us) const {
    return _side[us];
}

Bitboard Pieces::get(Piece piece) const {
    assert(piece < PieceCNT);
    return _bitboards[piece];
}

Bitboard Pieces::get(Side us, PieceType type) const {
    assert(type < PieceTypeCNT);
    Piece piece = types::piece_of(us, type);
    return _bitboards[piece];
}

void Pieces::put(Square sq, Piece piece) {
    assert(piece < PieceCNT);
    assert(_board[sq] == PieceCNT);
    Side us = types::side_of(piece);

    _board[sq] = piece;
    types::set_square(_all, sq);
    types::set_square(_side[us], sq);
    types::set_square(_bitboards[piece], sq);
}

Piece Pieces::remove(Square sq) {
    assert(_board[sq] < PieceCNT);
    Piece piece = _board[sq];
    Side us = types::side_of(piece);

    _board[sq] = PieceCNT;
    types::unset_square(_all, sq);
    types::unset_square(_side[us], sq);
    types::unset_square(_bitboards[piece], sq);

    return piece;
}

void Pieces::move(Square from, Square to) {
    Piece piece = remove(from);
    put(to, piece);
}

} // namespace cheslib
