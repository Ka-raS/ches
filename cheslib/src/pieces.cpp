#include "pieces.hpp"

namespace cheslib {

Pieces::Pieces(std::array<Piece, SquareCNT> &&board) : _board(std::move(board)), _bitboards{0}, _side{0}, _all(0) {
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

const std::array<Piece, SquareCNT> &Pieces::board() const {
    return _board;
}

Piece Pieces::at(Square sq) const {
    assert(sq < SquareCNT);
    return _board[sq];
}

Bitboard Pieces::all() const {
    return _all;
}

Bitboard Pieces::all_of(Side us) const {
    return _side[us];
}

Bitboard Pieces::get(Piece piece) const {
    return _bitboards[piece];
}

void Pieces::put(const Square sq, const Piece piece) {
    assert(piece < PieceCNT);
    assert(_board[sq] == PieceCNT);
    Side us = types::side_of(piece);

    _board[sq] = piece;
    types::set_square(_all, sq);
    types::set_square(_side[us], sq);
    types::set_square(_bitboards[piece], sq);
}

Piece Pieces::remove(const Square sq) {
    const Piece piece = _board[sq];
    assert(piece < PieceCNT);
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
