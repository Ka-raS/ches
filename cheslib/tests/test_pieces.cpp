#include <bit>

#include <catch2/catch_test_macros.hpp>

#include "pieces.hpp"
#include "utils.hpp"

using namespace cheslib;

TEST_CASE("Pieces: default constructor", "[pieces]") {
    const Pieces pieces{};
    CHECK(pieces.all() == 0);
    CHECK(pieces.all<White>() == 0);
    CHECK(pieces.all<Black>() == 0);
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        CHECK(pieces.at(sq) == PieceCNT);
    }
    for (Piece p : pieces.board()) {
        CHECK(p == PieceCNT);
    }
}

TEST_CASE("Pieces: initial position", "[pieces]") {
    const Pieces pieces = Pieces::initial();

    CHECK(std::popcount(pieces.all()) == 32);
    CHECK(std::popcount(pieces.all<White>()) == 16);
    CHECK(std::popcount(pieces.all<Black>()) == 16);

    CHECK(pieces.at(SquareE1) == WhiteKing);
    CHECK(pieces.at(SquareE8) == BlackKing);
    CHECK(pieces.at(SquareD1) == WhiteQueen);
    CHECK(pieces.at(SquareD8) == BlackQueen);
    CHECK(pieces.at(SquareA1) == WhiteRook);
    CHECK(pieces.at(SquareH1) == WhiteRook);
    CHECK(pieces.at(SquareA8) == BlackRook);
    CHECK(pieces.at(SquareH8) == BlackRook);
    CHECK(pieces.get(WhiteKing) == utils::bitboard_of(SquareE1));
    CHECK(pieces.get(BlackKing) == utils::bitboard_of(SquareE8));

    for (File f = FileA; f <= FileH; ++f) {
        Square wPawnSq = utils::square_of(f, Rank2);
        Square bPawnSq = utils::square_of(f, Rank7);
        CHECK(pieces.at(wPawnSq) == WhitePawn);
        CHECK(pieces.at(bPawnSq) == BlackPawn);
    }

    CHECK(pieces.count(WhitePawn) == 8);
    CHECK(pieces.count(BlackPawn) == 8);
    CHECK(pieces.get<White>(Pawn) == utils::bitboard_of(Rank2));
    CHECK(pieces.get<Black>(Pawn) == utils::bitboard_of(Rank7));
}

TEST_CASE("Pieces: custom board construction", "[pieces]") {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);
    board[SquareE4] = WhiteKnight;
    board[SquareD5] = BlackQueen;

    Pieces pieces(std::move(board));

    CHECK(std::popcount(pieces.all()) == 2);
    CHECK(std::popcount(pieces.all<White>()) == 1);
    CHECK(std::popcount(pieces.all<Black>()) == 1);
    CHECK(pieces.at(SquareE4) == WhiteKnight);
    CHECK(pieces.at(SquareD5) == BlackQueen);
    CHECK(pieces.get(WhiteKnight) == utils::bitboard_of(SquareE4));
    CHECK(pieces.get(BlackQueen) == utils::bitboard_of(SquareD5));
}

TEST_CASE("Pieces: put and remove operations", "[pieces]") {
    Pieces pieces;

    SECTION("Put a white piece on empty board") {
        pieces.put<White>(SquareE4, WhiteBishop);
        CHECK(pieces.at(SquareE4) == WhiteBishop);
        CHECK(std::popcount(pieces.all()) == 1);
        CHECK(std::popcount(pieces.all<White>()) == 1);
        CHECK(std::popcount(pieces.all<Black>()) == 0);
        CHECK(pieces.get(WhiteBishop) == utils::bitboard_of(SquareE4));
    }

    SECTION("Put a black piece on empty board") {
        pieces.put<Black>(SquareD5, BlackRook);
        CHECK(pieces.at(SquareD5) == BlackRook);
        CHECK(std::popcount(pieces.all()) == 1);
        CHECK(std::popcount(pieces.all<White>()) == 0);
        CHECK(std::popcount(pieces.all<Black>()) == 1);
        CHECK(pieces.get(BlackRook) == utils::bitboard_of(SquareD5));
    }

    SECTION("Remove a piece") {
        pieces.put<White>(SquareE4, WhiteBishop);
        Piece removed = pieces.remove<White>(SquareE4);
        CHECK(removed == WhiteBishop);
        CHECK(pieces.at(SquareE4) == PieceCNT);
        CHECK(std::popcount(pieces.all()) == 0);
        CHECK(std::popcount(pieces.all<White>()) == 0);
        CHECK(pieces.get(WhiteBishop) == 0);
    }

    SECTION("Put multiple pieces, then remove one") {
        pieces.put<White>(SquareE4, WhiteBishop);
        pieces.put<Black>(SquareD5, BlackRook);
        pieces.put<White>(SquareC3, WhiteKnight);
        CHECK(std::popcount(pieces.all()) == 3);
        pieces.remove<White>(SquareE4);
        CHECK(std::popcount(pieces.all()) == 2);
        CHECK(pieces.at(SquareE4) == PieceCNT);
        CHECK(pieces.get(WhiteBishop) == 0);
        CHECK(pieces.get(WhiteKnight) == utils::bitboard_of(SquareC3));
    }
}

TEST_CASE("Pieces: move operation", "[pieces]") {
    Pieces pieces{};
    pieces.put<White>(SquareE2, WhitePawn);
    pieces.put<Black>(SquareE7, BlackPawn);

    SECTION("Move white pawn forward") {
        pieces.move<White>(SquareE2, SquareE4);
        CHECK(pieces.at(SquareE2) == PieceCNT);
        CHECK(pieces.at(SquareE4) == WhitePawn);
        CHECK(std::popcount(pieces.all<White>()) == 1);
        CHECK(std::popcount(pieces.all<Black>()) == 1);
        CHECK(pieces.get(WhitePawn) == utils::bitboard_of(SquareE4));
    }

    SECTION("Move black pawn forward") {
        pieces.move<Black>(SquareE7, SquareE5);
        CHECK(pieces.at(SquareE7) == PieceCNT);
        CHECK(pieces.at(SquareE5) == BlackPawn);
        CHECK(std::popcount(pieces.all<Black>()) == 1);
        CHECK(pieces.get(BlackPawn) == utils::bitboard_of(SquareE5));
    }
}

TEST_CASE("Pieces: bitboard consistency after multiple operations", "[pieces]") {
    Pieces pieces = Pieces::initial();

    // remove a white pawn
    pieces.remove<White>(SquareE2);
    CHECK(std::popcount(pieces.all<White>()) == 15);
    CHECK(std::popcount(pieces.all()) == 31);
    CHECK(pieces.get(WhitePawn) == (utils::bitboard_of(Rank2) & ~utils::bitboard_of(SquareE2)));

    // move a black knight
    pieces.move<Black>(SquareB8, SquareC6);
    CHECK(pieces.at(SquareB8) == PieceCNT);
    CHECK(pieces.at(SquareC6) == BlackKnight);
    CHECK(pieces.get(BlackKnight) == utils::bitboard_of(SquareC6, SquareG8));
}

TEST_CASE("Pieces: alternating pawns", "[pieces]") {
    Pieces pieces;

    // put pawns
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        if (utils::rank_of(sq) % 2 == 0) {
            pieces.put<White>(sq, WhitePawn);
        } else {
            pieces.put<Black>(sq, BlackPawn);
        }
    }

    CHECK(std::popcount(pieces.all()) == 64);
    CHECK(std::popcount(pieces.all<White>()) == 32);
    CHECK(std::popcount(pieces.all<Black>()) == 32);
    CHECK(pieces.get(WhitePawn) == utils::bitboard_of(Rank1, Rank3, Rank5, Rank7));
    CHECK(pieces.get(BlackPawn) == utils::bitboard_of(Rank2, Rank4, Rank6, Rank8));

    // remove pawns
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        if (utils::rank_of(sq) % 2 == 0) {
            pieces.remove<White>(sq);
        } else {
            pieces.remove<Black>(sq);
        }
    }

    CHECK(std::popcount(pieces.all()) == 0);
    CHECK(pieces.all<White>() == 0);
    CHECK(pieces.all<Black>() == 0);

    for (Piece p = Piece(0); p < PieceCNT; ++p) {
        CHECK(pieces.get(p) == 0);
    }
}

TEST_CASE("Pieces: constepxr", "[pieces]") {
    constexpr Pieces pieces = []() {
        std::array<Piece, SquareCNT> board;
        board.fill(PieceCNT);
        board[SquareE4] = WhiteKnight;
        board[SquareD5] = BlackQueen;
        return Pieces(std::move(board));
    }();

    STATIC_CHECK(std::popcount(pieces.all()) == 2);
    STATIC_CHECK(std::popcount(pieces.all<White>()) == 1);
    STATIC_CHECK(std::popcount(pieces.all<Black>()) == 1);
    STATIC_CHECK(pieces.at(SquareE4) == WhiteKnight);
    STATIC_CHECK(pieces.at(SquareD5) == BlackQueen);
    STATIC_CHECK(pieces.get(WhiteKnight) == utils::bitboard_of(SquareE4));
    STATIC_CHECK(pieces.get(BlackQueen) == utils::bitboard_of(SquareD5));
}