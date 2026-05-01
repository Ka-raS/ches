#include "pieces.hpp"

namespace cheslib {

Pieces::Pieces(std::array<Piece, SquareCNT> &&board) : _board(std::move(board)), _bitboards{0}, _side{0}, _all(0) {
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        Piece piece = _board[sq];
        if (piece < PieceCNT) {
            Side side = types::side_of(piece);
            types::set_square(_all, sq);
            types::set_square(_side[side], sq);
            types::set_square(_bitboards[piece], sq);
        }
    }
}

const std::array<Piece, SquareCNT> &Pieces::board() const {
    return _board;
}

Piece Pieces::at(Square sq) const {
    assert(sq < SquareCNT);
    return _board[sq];
}

template <Side Us>
Bitboard Pieces::all() const {
    return _side[Us];
}

Bitboard Pieces::all() const {
    return _all;
}

template <Side Us>
Bitboard Pieces::get(PieceType type) const {
    Piece piece = types::piece_of<Us>(type);
    return _bitboards[piece];
}

template <Side Us>
void Pieces::put(const Square sq, const Piece piece) {
    assert(piece < PieceCNT);
    assert(types::side_of(piece) == Us);
    assert(_board[sq] == PieceCNT);

    _board[sq] = piece;
    types::set_square(_all, sq);
    types::set_square(_side[Us], sq);
    types::set_square(_bitboards[piece], sq);
}

template <Side Us>
Piece Pieces::remove(const Square sq) {
    const Piece piece = _board[sq];
    assert(piece < PieceCNT);
    assert(types::side_of(piece) == Us);

    _board[sq] = PieceCNT;
    types::unset_square(_all, sq);
    types::unset_square(_side[Us], sq);
    types::unset_square(_bitboards[piece], sq);

    return piece;
}

template <Side Us>
void Pieces::move(Square from, Square to) {
    Piece piece = remove<Us>(from);
    put<Us>(to, piece);
}

template Bitboard Pieces::all<White>() const;
template Bitboard Pieces::all<Black>() const;
template Bitboard Pieces::get<White>(PieceType) const;
template Bitboard Pieces::get<Black>(PieceType) const;
template void Pieces::put<White>(Square, Piece);
template void Pieces::put<Black>(Square, Piece);
template void Pieces::move<White>(Square, Square);
template void Pieces::move<Black>(Square, Square);
template Piece Pieces::remove<White>(Square);
template Piece Pieces::remove<Black>(Square);

} // namespace cheslib
