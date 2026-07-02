#include "pieces.hpp"

namespace cheslib {

Pieces::Pieces(const std::array<Piece, SquareCNT> &board) :
    _board{board},
    _bitboards{0},
    _side{0},
    _all{0} {
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        const Piece piece = _board[sq];
        if (piece >= PieceCNT) {
            continue;
        }

        const Side side = side_of(piece);
        set_square(_all, sq);
        set_square(_side[side], sq);
        set_square(_bitboards[piece], sq);
    }
}

std::array<Piece, SquareCNT> Pieces::initial() {
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

Piece Pieces::at(const Square square) const {
    assert(square < SquareCNT);
    return _board[square];
}

int Pieces::count(const Piece piece) const {
    assert(piece < PieceCNT);
    return std::popcount(_bitboards[piece]);
}

Square Pieces::king_of(const Side us) const {
    const Piece king = piece_of(us, King);
    const Bitboard king_bb = _bitboards[king];
    assert(king_bb != 0);
    return (Square)std::countr_zero(king_bb);
}

Bitboard Pieces::all() const {
    return _all;
}

Bitboard Pieces::all_of(const Side us) const {
    return _side[us];
}

Bitboard Pieces::get(const Piece piece) const {
    assert(piece < PieceCNT);
    return _bitboards[piece];
}

Bitboard Pieces::get(const Side us, const PieceType type) const {
    assert(type < PieceTypeCNT);
    const Piece piece = piece_of(us, type);
    return _bitboards[piece];
}

void Pieces::put(const Piece piece, const Square at) {
    assert(piece < PieceCNT);
    assert(_board[at] == PieceCNT);
    const Side us = side_of(piece);

    _board[at] = piece;
    set_square(_all, at);
    set_square(_side[us], at);
    set_square(_bitboards[piece], at);
}

Piece Pieces::remove(const Square at) {
    assert(_board[at] < PieceCNT);
    const Piece piece = _board[at];
    const Side us = side_of(piece);

    _board[at] = PieceCNT;
    unset_square(_all, at);
    unset_square(_side[us], at);
    unset_square(_bitboards[piece], at);

    return piece;
}

void Pieces::move(const Square from, const Square to) {
    const Piece piece = remove(from);
    put(piece, to);
}

} // namespace cheslib
