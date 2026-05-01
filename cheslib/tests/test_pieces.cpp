#include <catch2/catch_test_macros.hpp>

#include "pieces.hpp"

using namespace cheslib;

namespace {

const Pieces InitPieces = Pieces::initial();
const Pieces NoPieces = []() {
    std::array<Piece, SquareCNT> board{};
    board.fill(PieceCNT);
    return board;
}();

int count(const Pieces &pieces, Piece piece) {
    Bitboard bb = types::side_of(piece) == White ? pieces.get<White>(types::type_of(piece))
                                                 : pieces.get<Black>(types::type_of(piece));
    return std::popcount(bb);
}

} // namespace

TEST_CASE("Pieces: default constructor", "[pieces]") {
    CHECK(NoPieces.all() == 0);
    CHECK(NoPieces.all<White>() == 0);
    CHECK(NoPieces.all<Black>() == 0);
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        CHECK(NoPieces.at(sq) == PieceCNT);
    }
    for (Piece p : NoPieces.board()) {
        CHECK(p == PieceCNT);
    }
}

TEST_CASE("Pieces: initial position", "[pieces]") {
    const Pieces pieces = InitPieces;

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
    CHECK(pieces.get<White>(King) == types::bitboard_of(SquareE1));
    CHECK(pieces.get<Black>(King) == types::bitboard_of(SquareE8));

    for (File f = FileA; f <= FileH; ++f) {
        Square wPawnSq = types::square_of(f, Rank2);
        Square bPawnSq = types::square_of(f, Rank7);
        CHECK(pieces.at(wPawnSq) == WhitePawn);
        CHECK(pieces.at(bPawnSq) == BlackPawn);
    }

    CHECK(count(pieces, WhitePawn) == 8);
    CHECK(count(pieces, BlackPawn) == 8);
    CHECK(pieces.get<White>(Pawn) == types::bitboard_of(Rank2));
    CHECK(pieces.get<Black>(Pawn) == types::bitboard_of(Rank7));
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
    CHECK(pieces.get<White>(Knight) == types::bitboard_of(SquareE4));
    CHECK(pieces.get<Black>(Queen) == types::bitboard_of(SquareD5));
}

TEST_CASE("Pieces: put and remove operations", "[pieces]") {
    Pieces pieces = NoPieces;

    SECTION("Put a white piece on empty board") {
        pieces.put<White>(SquareE4, WhiteBishop);
        CHECK(pieces.at(SquareE4) == WhiteBishop);
        CHECK(std::popcount(pieces.all()) == 1);
        CHECK(std::popcount(pieces.all<White>()) == 1);
        CHECK(std::popcount(pieces.all<Black>()) == 0);
        CHECK(pieces.get<White>(Bishop) == types::bitboard_of(SquareE4));
    }

    SECTION("Put a black piece on empty board") {
        pieces.put<Black>(SquareD5, BlackRook);
        CHECK(pieces.at(SquareD5) == BlackRook);
        CHECK(std::popcount(pieces.all()) == 1);
        CHECK(std::popcount(pieces.all<White>()) == 0);
        CHECK(std::popcount(pieces.all<Black>()) == 1);
        CHECK(pieces.get<Black>(Rook) == types::bitboard_of(SquareD5));
    }

    SECTION("Remove a piece") {
        pieces.put<White>(SquareE4, WhiteBishop);
        Piece removed = pieces.remove<White>(SquareE4);
        CHECK(removed == WhiteBishop);
        CHECK(pieces.at(SquareE4) == PieceCNT);
        CHECK(std::popcount(pieces.all()) == 0);
        CHECK(std::popcount(pieces.all<White>()) == 0);
        CHECK(pieces.get<White>(Bishop) == 0);
    }

    SECTION("Put multiple pieces, then remove one") {
        pieces.put<White>(SquareE4, WhiteBishop);
        pieces.put<Black>(SquareD5, BlackRook);
        pieces.put<White>(SquareC3, WhiteKnight);
        CHECK(std::popcount(pieces.all()) == 3);
        pieces.remove<White>(SquareE4);
        CHECK(std::popcount(pieces.all()) == 2);
        CHECK(pieces.at(SquareE4) == PieceCNT);
        CHECK(pieces.get<White>(Bishop) == 0);
        CHECK(pieces.get<White>(Knight) == types::bitboard_of(SquareC3));
    }
}

TEST_CASE("Pieces: move operation", "[pieces]") {
    Pieces pieces = NoPieces;
    pieces.put<White>(SquareE2, WhitePawn);
    pieces.put<Black>(SquareE7, BlackPawn);

    SECTION("Move white pawn forward") {
        pieces.move<White>(SquareE2, SquareE4);
        CHECK(pieces.at(SquareE2) == PieceCNT);
        CHECK(pieces.at(SquareE4) == WhitePawn);
        CHECK(std::popcount(pieces.all<White>()) == 1);
        CHECK(std::popcount(pieces.all<Black>()) == 1);
        CHECK(pieces.get<White>(Pawn) == types::bitboard_of(SquareE4));
    }

    SECTION("Move black pawn forward") {
        pieces.move<Black>(SquareE7, SquareE5);
        CHECK(pieces.at(SquareE7) == PieceCNT);
        CHECK(pieces.at(SquareE5) == BlackPawn);
        CHECK(std::popcount(pieces.all<Black>()) == 1);
        CHECK(pieces.get<Black>(Pawn) == types::bitboard_of(SquareE5));
    }
}

TEST_CASE("Pieces: bitboard consistency after multiple operations", "[pieces]") {
    Pieces pieces = InitPieces;

    // remove a white pawn
    pieces.remove<White>(SquareE2);
    CHECK(std::popcount(pieces.all<White>()) == 15);
    CHECK(std::popcount(pieces.all()) == 31);
    CHECK(pieces.get<White>(Pawn) == (types::bitboard_of(Rank2) & ~types::bitboard_of(SquareE2)));

    // move a black knight
    pieces.move<Black>(SquareB8, SquareC6);
    CHECK(pieces.at(SquareB8) == PieceCNT);
    CHECK(pieces.at(SquareC6) == BlackKnight);
    CHECK(pieces.get<Black>(Knight) == types::bitboard_of(SquareC6, SquareG8));
}

TEST_CASE("Pieces: alternating pawns", "[pieces]") {
    Pieces pieces = NoPieces;

    // put pawns
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        if (types::rank_of(sq) % 2 == 0) {
            pieces.put<White>(sq, WhitePawn);
        } else {
            pieces.put<Black>(sq, BlackPawn);
        }
    }

    CHECK(std::popcount(pieces.all()) == 64);
    CHECK(std::popcount(pieces.all<White>()) == 32);
    CHECK(std::popcount(pieces.all<Black>()) == 32);
    CHECK(pieces.get<White>(Pawn) == types::bitboard_of(Rank1, Rank3, Rank5, Rank7));
    CHECK(pieces.get<Black>(Pawn) == types::bitboard_of(Rank2, Rank4, Rank6, Rank8));

    // remove pawns
    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        if (types::rank_of(sq) % 2 == 0) {
            pieces.remove<White>(sq);
        } else {
            pieces.remove<Black>(sq);
        }
    }

    CHECK(std::popcount(pieces.all()) == 0);
    CHECK(pieces.all<White>() == 0);
    CHECK(pieces.all<Black>() == 0);

    for (PieceType type = Pawn; type <= King; ++type) {
        CHECK(pieces.get<White>(type) == 0);
        CHECK(pieces.get<Black>(type) == 0);
    }
}
