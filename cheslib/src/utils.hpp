#pragma once

#include <bit>
#include <cassert>
#include <cstdlib>

#include "cheslib/types.hpp"

namespace cheslib {

/**
 * `uint64_t` represents 64 squares on chessboard
 * if bit=1 then a piece type is on the square
 * see: https://www.chessprogramming.org/Bitboards
 */
using Bitboard = uint64_t;

enum Direction : int8_t {
    North = SquareA2 - SquareA1,
    East = SquareB1 - SquareA1,
    South = -North,
    West = -East,

    NorthEast = North + East,
    SouthEast = South + East,
    SouthWest = -NorthEast,
    NorthWest = -SouthEast
};

namespace utils {

template <Side Us>
constexpr Square square_behind(Square sq);
constexpr Square square_of(File file, Rank rank);
constexpr Square pop_lsb(Bitboard &bb);

constexpr Bitboard bitboard_of(std::same_as<Rank> auto... ranks);
constexpr Bitboard bitboard_of(std::same_as<File> auto... files);
constexpr Bitboard bitboard_of(std::same_as<Square> auto... squares);

constexpr bool has_square(Bitboard bb, std::same_as<Square> auto... squares);
constexpr void set_square(Bitboard &bb, std::same_as<Square> auto... squares);
constexpr void unset_square(Bitboard &bb, std::same_as<Square> auto... squares);

template <Side Us>
constexpr Piece piece_of(PieceType type);
constexpr PieceType type_of(Piece piece);
constexpr File file_of(Square sq);
constexpr Rank rank_of(Square sq);
constexpr Side side_of(Piece piece);

// implementation

template <Side Us>
constexpr Square square_behind(Square sq) {
    if constexpr (Us == White) {
        assert(sq >= SquareA2);
    } else {
        assert(sq <= SquareH7);
    }

    constexpr Direction backward = (Us == White) ? South : North;
    return Square(sq + int(backward));
}

constexpr Square square_of(File file, Rank rank) {
    assert(file < FileCNT);
    assert(rank < RankCNT);
    return Square(rank << 3 | file); // rank * FileCNT + file
}

constexpr Square pop_lsb(Bitboard &bb) {
    Square sq = Square(std::countr_zero(bb));
    bb &= (bb - 1);
    return sq;
}

constexpr Bitboard bitboard_of(std::same_as<Square> auto... squares) {
    (assert(squares < SquareCNT), ...);
    return ((1ULL << squares) | ...);
}

constexpr Bitboard bitboard_of(std::same_as<Rank> auto... ranks) {
    (assert(ranks < RankCNT), ...);
    constexpr Bitboard rank_1 =
        bitboard_of(SquareA1, SquareB1, SquareC1, SquareD1, SquareE1, SquareF1, SquareG1, SquareH1);

    //       rank_1 << padding_squares
    return ((rank_1 << (ranks << 3)) | ...);
}

constexpr Bitboard bitboard_of(std::same_as<File> auto... files) {
    (assert(files < FileCNT), ...);
    constexpr Bitboard file_a =
        bitboard_of(SquareA1, SquareA2, SquareA3, SquareA4, SquareA5, SquareA6, SquareA7, SquareA8);
    return ((file_a << files) | ...);
}

constexpr bool has_square(Bitboard bb, std::same_as<Square> auto... squares) {
    (assert(squares < SquareCNT), ...);
    return (((bb >> squares) & 1) && ...);
}

constexpr void set_square(Bitboard &bb, std::same_as<Square> auto... squares) {
    bb |= bitboard_of(squares...);
}

constexpr void unset_square(Bitboard &bb, std::same_as<Square> auto... squares) {
    bb &= ~bitboard_of(squares...);
}

template <Side Us>
constexpr Piece piece_of(PieceType type) {
    assert(type < PieceTypeCNT);
    constexpr unsigned offset = (Us == White) ? 0 : PieceTypeCNT;
    return Piece(type + offset);
}

constexpr PieceType type_of(Piece piece) {
    assert(piece < PieceCNT);
    unsigned offset = (piece < BlackPawn) ? 0 : BlackPawn;
    return PieceType(piece - offset);
}

constexpr File file_of(Square sq) {
    assert(sq < SquareCNT);
    return File(sq & 7); // sq % 8
}

constexpr Rank rank_of(Square sq) {
    assert(sq < SquareCNT);
    return Rank(sq >> 3); // sq / 8
}

constexpr Side side_of(Piece piece) {
    assert(piece < PieceCNT);
    return Side(piece >= BlackPawn);
}

} // namespace utils

} // namespace cheslib
