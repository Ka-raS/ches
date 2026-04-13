#pragma once

#include <bit>
#include <cassert>
#include <cstdlib>

#include "cheslib/types.hpp"

namespace cheslib {

constexpr bool has_square(Bitboard bb, Square sq) {
    assert(sq < SquareCNT);
    return bb & (1ULL << sq);
}

constexpr void set_square(Bitboard &bb, Square sq) {
    assert(sq < SquareCNT);
    bb |= (1ULL << sq);
}

constexpr void unset_square(Bitboard &bb, Square sq) {
    assert(sq < SquareCNT);
    bb &= ~(1ULL << sq);
}

constexpr File file_of(Square sq) {
    assert(sq < SquareCNT);
    return File(sq & 7); // sq % 8
}

constexpr Rank rank_of(Square sq) {
    assert(sq < SquareCNT);
    return Rank(sq >> 3); // sq / 8
}

constexpr Square pop_lsb(Bitboard &bb) {
    Square sq = Square(std::countr_zero(bb));
    bb &= (bb - 1);
    return sq;
}

template <Side Us>
constexpr Square square_behind(Square sq) {
    assert(SquareH1 < sq && sq < SquareCNT);
    constexpr Direction backward = (Us == White) ? Down : Up;
    return Square(sq + int(backward));
}

constexpr Square square_behind(Square sq, Side side) {
    assert(SquareH1 < sq && sq < SquareCNT);
    Direction backward = (side == White) ? Down : Up;
    return Square(sq + (int)backward);
}

constexpr Square to_square(File file, Rank rank) {
    assert(file < FileCNT);
    assert(rank < RankCNT);
    return Square(rank << 3 | file); // rank * 8 + file
}

template <typename... Args>
    requires(std::is_same_v<Args, Square> && ...)
constexpr Bitboard to_bitboard(Args... squares) {
    Bitboard bb = 0;
    (set_square(bb, squares), ...);
    return bb;
}

constexpr Bitboard rank_bitboard(Rank rank) {
    assert(rank < RankCNT);
    constexpr Bitboard rank_1 =
        to_bitboard(SquareA1, SquareB1, SquareC1, SquareD1, SquareE1, SquareF1, SquareG1, SquareH1);

    int padding_squares = rank << 3; // rank * 8
    return rank_1 << padding_squares;
}

constexpr Bitboard file_bitboard(File file) {
    assert(file < FileCNT);
    constexpr Bitboard file_a =
        to_bitboard(SquareA1, SquareA2, SquareA3, SquareA4, SquareA5, SquareA6, SquareA7, SquareA8);

    return file_a << file;
}

template <Side Us>
constexpr Piece piece_of(PieceType type) {
    assert(type < PieceTypeCNT);
    constexpr int offset = (Us == White) ? 0 : PieceTypeCNT;
    return Piece(type + offset);
}

constexpr Piece piece_of(PieceType type, Side side) {
    assert(type < PieceTypeCNT);
    return Piece(type + side * (int)PieceTypeCNT);
}

constexpr Side side_of(Piece piece) {
    assert(piece < PieceCNT);
    return Side(piece >= BlackPawn);
}

constexpr PieceType type_of(Piece piece) {
    assert(piece < PieceCNT);
    return PieceType(piece % (int)PieceTypeCNT);
}

} // namespace cheslib
