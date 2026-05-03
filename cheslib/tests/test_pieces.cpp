#include <catch2/catch_test_macros.hpp>

#include "pieces.hpp"

using namespace cheslib;

namespace {

const Pieces NoPieces = []() {
    std::array<Piece, SquareCNT> board{};
    board.fill(PieceCNT);
    return board;
}();

const Pieces InitPieces = []() {
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
}();

int count(Bitboard bb) {
    return std::popcount(bb);
}

int count(const Pieces &pieces, Piece piece) {
    Bitboard bb = pieces.get(piece);
    return count(bb);
}

} // namespace

TEST_CASE("Pieces: default constructor", "[pieces]") {
    CHECK(NoPieces.all() == 0);
    CHECK(NoPieces.all_of(White) == 0);
    CHECK(NoPieces.all_of(Black) == 0);
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        CHECK(NoPieces.at(sq) == PieceCNT);
    }
    for (Piece p : NoPieces.board()) {
        CHECK(p == PieceCNT);
    }
}

TEST_CASE("Pieces: initial position", "[pieces]") {
    const Pieces pieces = InitPieces;

    CHECK(count(pieces.all()) == 32);
    CHECK(count(pieces.all_of(White)) == 16);
    CHECK(count(pieces.all_of(Black)) == 16);

    CHECK(pieces.at(SquareE1) == WhiteKing);
    CHECK(pieces.at(SquareE8) == BlackKing);
    CHECK(pieces.at(SquareD1) == WhiteQueen);
    CHECK(pieces.at(SquareD8) == BlackQueen);
    CHECK(pieces.at(SquareA1) == WhiteRook);
    CHECK(pieces.at(SquareH1) == WhiteRook);
    CHECK(pieces.at(SquareA8) == BlackRook);
    CHECK(pieces.at(SquareH8) == BlackRook);
    CHECK(pieces.get(WhiteKing) == types::bitboard_of(SquareE1));
    CHECK(pieces.get(BlackKing) == types::bitboard_of(SquareE8));

    for (File f = FileA; f <= FileH; ++f) {
        Square wPawnSq = types::square_of(f, Rank2);
        Square bPawnSq = types::square_of(f, Rank7);
        CHECK(pieces.at(wPawnSq) == WhitePawn);
        CHECK(pieces.at(bPawnSq) == BlackPawn);
    }

    CHECK(count(pieces, WhitePawn) == 8);
    CHECK(count(pieces, BlackPawn) == 8);
    CHECK(pieces.get(WhitePawn) == types::bitboard_of(Rank2));
    CHECK(pieces.get(BlackPawn) == types::bitboard_of(Rank7));
}

TEST_CASE("Pieces: custom board construction", "[pieces]") {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);
    board[SquareE4] = WhiteKnight;
    board[SquareD5] = BlackQueen;

    Pieces pieces(std::move(board));

    CHECK(count(pieces.all()) == 2);
    CHECK(count(pieces.all_of(White)) == 1);
    CHECK(count(pieces.all_of(Black)) == 1);
    CHECK(pieces.at(SquareE4) == WhiteKnight);
    CHECK(pieces.at(SquareD5) == BlackQueen);
    CHECK(pieces.get(WhiteKnight) == types::bitboard_of(SquareE4));
    CHECK(pieces.get(BlackQueen) == types::bitboard_of(SquareD5));
}

TEST_CASE("Pieces: put and remove operations", "[pieces]") {
    Pieces pieces = NoPieces;

    SECTION("Put a white piece on empty board") {
        pieces.put(SquareE4, WhiteBishop);
        CHECK(pieces.at(SquareE4) == WhiteBishop);
        CHECK(count(pieces.all()) == 1);
        CHECK(count(pieces.all_of(White)) == 1);
        CHECK(count(pieces.all_of(Black)) == 0);
        CHECK(pieces.get(WhiteBishop) == types::bitboard_of(SquareE4));
    }

    SECTION("Put a black piece on empty board") {
        pieces.put(SquareD5, BlackRook);
        CHECK(pieces.at(SquareD5) == BlackRook);
        CHECK(count(pieces.all()) == 1);
        CHECK(count(pieces.all_of(White)) == 0);
        CHECK(count(pieces.all_of(Black)) == 1);
        CHECK(pieces.get(BlackRook) == types::bitboard_of(SquareD5));
    }

    SECTION("Remove a piece") {
        pieces.put(SquareE4, WhiteBishop);
        Piece removed = pieces.remove(SquareE4);
        CHECK(removed == WhiteBishop);
        CHECK(pieces.at(SquareE4) == PieceCNT);
        CHECK(count(pieces.all()) == 0);
        CHECK(count(pieces.all_of(White)) == 0);
        CHECK(pieces.get(WhiteBishop) == 0);
    }

    SECTION("Put multiple pieces, then remove one") {
        pieces.put(SquareE4, WhiteBishop);
        pieces.put(SquareD5, BlackRook);
        pieces.put(SquareC3, WhiteKnight);
        CHECK(count(pieces.all()) == 3);

        pieces.remove(SquareE4);
        CHECK(count(pieces.all()) == 2);
        CHECK(pieces.at(SquareE4) == PieceCNT);
        CHECK(pieces.get(WhiteBishop) == 0);
        CHECK(pieces.get(WhiteKnight) == types::bitboard_of(SquareC3));
    }
}

TEST_CASE("Pieces: move operation", "[pieces]") {
    Pieces pieces = NoPieces;
    pieces.put(SquareE2, WhitePawn);
    pieces.put(SquareE7, BlackPawn);

    SECTION("Move white pawn forward") {
        pieces.move(SquareE2, SquareE4);
        CHECK(pieces.at(SquareE2) == PieceCNT);
        CHECK(pieces.at(SquareE4) == WhitePawn);
        CHECK(count(pieces.all_of(White)) == 1);
        CHECK(count(pieces.all_of(Black)) == 1);
        CHECK(pieces.get(WhitePawn) == types::bitboard_of(SquareE4));
    }

    SECTION("Move black pawn forward") {
        pieces.move(SquareE7, SquareE5);
        CHECK(pieces.at(SquareE7) == PieceCNT);
        CHECK(pieces.at(SquareE5) == BlackPawn);
        CHECK(count(pieces.all_of(Black)) == 1);
        CHECK(pieces.get(BlackPawn) == types::bitboard_of(SquareE5));
    }
}

TEST_CASE("Pieces: bitboard consistency after multiple operations", "[pieces]") {
    Pieces pieces = InitPieces;

    // remove a white pawn
    pieces.remove(SquareE2);
    CHECK(count(pieces.all_of(White)) == 15);
    CHECK(count(pieces.all()) == 31);
    CHECK(pieces.get(WhitePawn) == (types::bitboard_of(Rank2) & ~types::bitboard_of(SquareE2)));

    // move a black knight
    pieces.move(SquareB8, SquareC6);
    CHECK(pieces.at(SquareB8) == PieceCNT);
    CHECK(pieces.at(SquareC6) == BlackKnight);
    CHECK(pieces.get(BlackKnight) == types::bitboard_of(SquareC6, SquareG8));
}

TEST_CASE("Pieces: alternating pawns", "[pieces]") {
    Pieces pieces = NoPieces;

    // put pawns
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        if (types::rank_of(sq) % 2 == 0) {
            pieces.put(sq, WhitePawn);
        } else {
            pieces.put(sq, BlackPawn);
        }
    }

    CHECK(count(pieces.all()) == 64);
    CHECK(count(pieces.all_of(White)) == 32);
    CHECK(count(pieces.all_of(Black)) == 32);

    constexpr Bitboard expected_white =
        types::bitboard_of(Rank1) | types::bitboard_of(Rank3) | types::bitboard_of(Rank5) | types::bitboard_of(Rank7);

    CHECK(pieces.get(WhitePawn) == expected_white);
    CHECK(pieces.get(BlackPawn) == ~expected_white);

    // remove pawns
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        if (types::rank_of(sq) % 2 == 0) {
            pieces.remove(sq);
        } else {
            pieces.remove(sq);
        }
    }

    CHECK(count(pieces.all()) == 0);
    CHECK(pieces.all_of(White) == 0);
    CHECK(pieces.all_of(Black) == 0);

    for (Piece piece = Piece(0); piece < PieceCNT; ++piece) {
        CHECK(pieces.get(piece) == 0);
    }
}
