#include <catch2/catch_test_macros.hpp>

#include "types.hpp"

using namespace cheslib;
using namespace cheslib::types;

TEST_CASE("Types: square_behind", "[types]") {
    SECTION("White") {
        CHECK(square_behind(White, SquareA2) == SquareA1);
        CHECK(square_behind(White, SquareH8) == SquareH7);
        CHECK(square_behind(White, SquareD5) == SquareD4);
    }

    SECTION("Black") {
        CHECK(square_behind(Black, SquareA7) == SquareA8);
        CHECK(square_behind(Black, SquareH1) == SquareH2);
        CHECK(square_behind(Black, SquareE4) == SquareE5);
    }
}

TEST_CASE("Types: square_of", "[types]") {
    CHECK(square_of(FileA, Rank1) == SquareA1);
    CHECK(square_of(FileH, Rank8) == SquareH8);
    CHECK(square_of(FileD, Rank4) == SquareD4);
    CHECK(square_of(FileC, Rank2) == SquareC2);
}

TEST_CASE("Types: pop_lsb", "[types]") {
    Bitboard bb = (1ull << SquareD1) | (1ull << SquareF1);

    CHECK(pop_lsb(bb) == SquareD1);
    CHECK(bb == (1ull << SquareF1));

    CHECK(pop_lsb(bb) == SquareF1);
    CHECK(bb == 0);
}

TEST_CASE("Types: bitboard_of squares", "[types]") {
    CHECK(bitboard_of(SquareA1) == 1ull);
    CHECK(bitboard_of(SquareH8) == (1ull << SquareH8));
    CHECK(bitboard_of(SquareA1, SquareH8) == ((1ull << SquareA1) | (1ull << SquareH8)));
    CHECK(bitboard_of(SquareC3, SquareF6) == ((1ull << SquareC3) | (1ull << SquareF6)));
}

TEST_CASE("Types: bitboard_of rank", "[types]") {
    constexpr Bitboard rank_1 = 0xFFull;
    CHECK(bitboard_of(Rank1) == rank_1);
    CHECK(bitboard_of(Rank8) == (rank_1 << SquareA8));
    CHECK(bitboard_of(Rank4) == (rank_1 << SquareA4));
}

TEST_CASE("Types: bitboard_of files", "[types]") {
    constexpr Bitboard fileA = 0x0101010101010101ull;
    constexpr Bitboard fileH = 0x8080808080808080ull;
    CHECK(bitboard_of(FileA) == fileA);
    CHECK(bitboard_of(FileH) == fileH);
    CHECK(bitboard_of(FileD) == (fileA << FileD));
}

TEST_CASE("Types: has_square", "[types]") {
    const Bitboard bb = bitboard_of(SquareA1, SquareE4, SquareH8);
    CHECK(has_square(bb, SquareA1));
    CHECK(has_square(bb, SquareE4));
    CHECK(has_square(bb, SquareH8));
    CHECK_FALSE(has_square(bb, SquareA2));
    CHECK_FALSE(has_square(bb, SquareD4));
}

TEST_CASE("Types: set_square and unset_square", "[types]") {
    Bitboard bb = 0;
    set_square(bb, SquareB2);
    CHECK(bb == bitboard_of(SquareB2));

    set_square(bb, SquareG7);
    set_square(bb, SquareC3);
    CHECK(has_square(bb, SquareB2));
    CHECK(has_square(bb, SquareG7));
    CHECK(has_square(bb, SquareC3));

    unset_square(bb, SquareB2);
    CHECK_FALSE(has_square(bb, SquareB2));
    CHECK(has_square(bb, SquareG7));
    CHECK(has_square(bb, SquareC3));

    unset_square(bb, SquareG7);
    CHECK_FALSE(has_square(bb, SquareG7));
    CHECK(has_square(bb, SquareC3));

    unset_square(bb, SquareC3);
    CHECK(bb == 0);
}

TEST_CASE("Types: piece_of", "[types]") {
    CHECK(piece_of(White, Pawn) == WhitePawn);
    CHECK(piece_of(White, Knight) == WhiteKnight);
    CHECK(piece_of(White, Bishop) == WhiteBishop);
    CHECK(piece_of(White, Rook) == WhiteRook);
    CHECK(piece_of(White, Queen) == WhiteQueen);
    CHECK(piece_of(White, King) == WhiteKing);

    CHECK(piece_of(Black, Pawn) == BlackPawn);
    CHECK(piece_of(Black, Knight) == BlackKnight);
    CHECK(piece_of(Black, Bishop) == BlackBishop);
    CHECK(piece_of(Black, Rook) == BlackRook);
    CHECK(piece_of(Black, Queen) == BlackQueen);
    CHECK(piece_of(Black, King) == BlackKing);
}

TEST_CASE("Types: type_of", "[types]") {
    CHECK(type_of(WhitePawn) == Pawn);
    CHECK(type_of(WhiteKnight) == Knight);
    CHECK(type_of(WhiteBishop) == Bishop);
    CHECK(type_of(WhiteRook) == Rook);
    CHECK(type_of(WhiteQueen) == Queen);
    CHECK(type_of(WhiteKing) == King);

    CHECK(type_of(BlackPawn) == Pawn);
    CHECK(type_of(BlackKnight) == Knight);
    CHECK(type_of(BlackBishop) == Bishop);
    CHECK(type_of(BlackRook) == Rook);
    CHECK(type_of(BlackQueen) == Queen);
    CHECK(type_of(BlackKing) == King);
}

TEST_CASE("Types: file_of and rank_of", "[types]") {
    CHECK(file_of(SquareA1) == FileA);
    CHECK(file_of(SquareB3) == FileB);
    CHECK(file_of(SquareH8) == FileH);
    CHECK(file_of(SquareE5) == FileE);

    CHECK(rank_of(SquareA1) == Rank1);
    CHECK(rank_of(SquareB3) == Rank3);
    CHECK(rank_of(SquareH8) == Rank8);
    CHECK(rank_of(SquareE5) == Rank5);

    CHECK(file_of(SquareA8) == FileA);
    CHECK(rank_of(SquareA8) == Rank8);
    CHECK(file_of(SquareH1) == FileH);
    CHECK(rank_of(SquareH1) == Rank1);
}

TEST_CASE("Types: side_of", "[types]") {
    CHECK(side_of(WhitePawn) == White);
    CHECK(side_of(WhiteKnight) == White);
    CHECK(side_of(WhiteBishop) == White);
    CHECK(side_of(WhiteRook) == White);
    CHECK(side_of(WhiteQueen) == White);
    CHECK(side_of(WhiteKing) == White);

    CHECK(side_of(BlackPawn) == Black);
    CHECK(side_of(BlackKnight) == Black);
    CHECK(side_of(BlackBishop) == Black);
    CHECK(side_of(BlackRook) == Black);
    CHECK(side_of(BlackQueen) == Black);
    CHECK(side_of(BlackKing) == Black);
}

TEST_CASE("Types: constexpr", "[types]") {
    STATIC_CHECK(file_of(SquareH8) == FileH);
    STATIC_CHECK(rank_of(SquareD4) == Rank4);
    STATIC_CHECK(side_of(BlackPawn) == Black);
    STATIC_CHECK(type_of(BlackKnight) == Knight);
    STATIC_CHECK(piece_of(White, Queen) == WhiteQueen);
    STATIC_CHECK(square_of(FileA, Rank1) == SquareA1);
    STATIC_CHECK(square_behind(White, SquareA2) == SquareA1);
    STATIC_CHECK(square_behind(Black, SquareA7) == SquareA8);
    STATIC_CHECK(bitboard_of(Rank1) == 0xFFull);
    STATIC_CHECK(bitboard_of(FileA) == 0x0101010101010101ull);
}