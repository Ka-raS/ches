#pragma once

#include <array>
#include <bit>
#include <cassert>

#include "cheslib/types.hpp"
#include "utils.hpp"

namespace cheslib {

/**
 * TODO: write comments here
 * see: https://www.chessprogramming.org/Bitboard_Board-Definition
 */
class Pieces {
  public:
    constexpr Pieces();
    constexpr Pieces(std::array<Piece, SquareCNT> &&board);
    static constexpr Pieces initial();

    constexpr const std::array<Piece, SquareCNT> &board() const;
    constexpr Piece at(Square sq) const;
    constexpr int count(Piece piece) const;

    template <Side Us>
    constexpr Bitboard all() const;
    constexpr Bitboard all() const;

    template <Side Us>
    constexpr Bitboard get(PieceType type) const;
    constexpr Bitboard get(Piece piece) const;

    template <Side Us>
    constexpr void put(Square sq, Piece piece);
    template <Side Us>
    constexpr void move(Square from, Square to);
    template <Side Us>
    constexpr Piece remove(Square sq);

  private:
    std::array<Piece, SquareCNT> _board;
    Bitboard _bitboards[PieceCNT];
    Bitboard _side[2];
    Bitboard _all;
};

constexpr Pieces::Pieces() : _bitboards{0}, _side{0}, _all(0) {
    _board.fill(PieceCNT);
}

constexpr Pieces::Pieces(std::array<Piece, SquareCNT> &&board) : _board(std::move(board)), _bitboards{0}, _side{0}, _all(0) {
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        Piece piece = _board[sq];
        if (piece < PieceCNT) {
            Side side = utils::side_of(piece);
            utils::set_square(_all, sq);
            utils::set_square(_side[side], sq);
            utils::set_square(_bitboards[piece], sq);
        }
    }
}

constexpr Pieces Pieces::initial() {
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

    return Pieces(std::move(board));
}

constexpr const std::array<Piece, SquareCNT> &Pieces::board() const {
    return _board;
}

constexpr Piece Pieces::at(Square sq) const {
    assert(sq < SquareCNT);
    return _board[sq];
}

constexpr int Pieces::count(Piece piece) const {
    assert(piece < PieceCNT);
    return std::popcount(_bitboards[piece]);
}

constexpr Bitboard Pieces::all() const {
    return _all;
}

template <Side Us>
constexpr Bitboard Pieces::all() const {
    return _side[Us];
}

template <Side Us>
constexpr Bitboard Pieces::get(PieceType type) const {
    Piece piece = utils::piece_of<Us>(type);
    return _bitboards[piece];
}

constexpr Bitboard Pieces::get(Piece piece) const {
    assert(piece < PieceCNT);
    return _bitboards[piece];
}

template <Side Us>
constexpr void Pieces::put(Square sq, Piece piece) {
    assert(piece < PieceCNT);
    assert(utils::side_of(piece) == Us);
    assert(_board[sq] == PieceCNT);

    _board[sq] = piece;
    utils::set_square(_all, sq);
    utils::set_square(_side[Us], sq);
    utils::set_square(_bitboards[piece], sq);
}

template <Side Us>
constexpr Piece Pieces::remove(Square sq) {
    const Piece piece = _board[sq];
    assert(piece < PieceCNT);
    assert(utils::side_of(piece) == Us);

    _board[sq] = PieceCNT;
    utils::unset_square(_all, sq);
    utils::unset_square(_side[Us], sq);
    utils::unset_square(_bitboards[piece], sq);

    return piece;
}

template <Side Us>
constexpr void Pieces::move(Square from, Square to) {
    Piece piece = remove<Us>(from);
    put<Us>(to, piece);
}

} // namespace cheslib
