#include <catch2/catch_test_macros.hpp>

#include "movegen.hpp"

using namespace cheslib;

TEST_CASE("Movegen: Generate pseudo-legal moves from initial position", "[movegen]") {
    MoveList moves = movegen::pseudo_legals(Position::initial());

    CHECK(moves.size() == 20); // 16 pawn moves + 4 knight moves
}

TEST_CASE("Movegen: Generate pseudo-legal moves from empty board", "[movegen]") {
    auto board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        return b;
    };
    State state(NoCastles, FileCNT, White, 0);
    Position position(board(), state);
    MoveList moves = movegen::pseudo_legals(position);

    CHECK(moves.size() == 0);
}

TEST_CASE("Movegen: Generate pseudo-legal moves when only kings", "[movegen]") {
    auto board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE1] = WhiteKing;
        b[SquareE8] = BlackKing;
        return b;
    };
    State state(NoCastles, FileCNT, White, 0);
    Position position(board(), state);
    MoveList moves = movegen::pseudo_legals(position);

    CHECK(moves.size() == 5); // white king should have 5 moves
}

TEST_CASE("Movegen: Generate pseudo-legal moves when pawn promotion", "[movegen]") {
    auto board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE7] = WhitePawn;
        return b;
    };
    State state(NoCastles, FileCNT, White, 0);
    Position position(board(), state);
    MoveList moves = movegen::pseudo_legals(position);

    CHECK(moves.has({SquareE7, SquareE8, QueenPromo}));
}

TEST_CASE("Movegen: Initial position includes 8 double pawn pushes", "[movegen]") {
    MoveList moves = movegen::pseudo_legals(Position::initial());

    int double_pushes = 0;
    for (Move move : moves) {
        if (move.flag() == DoublePawnPush) {
            ++double_pushes;
        }
    }
    CHECK(double_pushes == 8);
}

TEST_CASE("Movegen: Blocked pawn cannot push one or two squares", "[movegen]") {
    auto board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE2] = WhitePawn;
        b[SquareE3] = BlackPawn; // blocker in front
        return b;
    };
    State state(NoCastles, FileCNT, White, 0);
    Position position(board(), state);
    MoveList moves = movegen::pseudo_legals(position);

    CHECK_FALSE(moves.has({SquareE2, SquareE3, QuietMove}));
    CHECK_FALSE(moves.has({SquareE2, SquareE4, DoublePawnPush}));
}

TEST_CASE("Movegen: White pawn diagonal captures are generated", "[movegen]") {
    auto board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE5] = WhitePawn;
        b[SquareD6] = b[SquareF6] = BlackPawn;
        return b;
    };
    State state(NoCastles, FileCNT, White, 0);
    Position position(board(), state);
    MoveList moves = movegen::pseudo_legals(position);

    CHECK(moves.has({SquareE5, SquareD6, Capture}));
    CHECK(moves.has({SquareE5, SquareF6, Capture}));
}

TEST_CASE("Movegen: En passant capture is generated when en passant file is set", "[movegen]") {
    auto board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE5] = WhitePawn;
        b[SquareD5] = BlackPawn;
        return b;
    };
    State state(NoCastles, FileD, White, 0);
    Position position(board(), state);
    MoveList moves = movegen::pseudo_legals(position);

    CHECK(moves.has({SquareE5, SquareD6, EnPassant}));
}

TEST_CASE("Movegen: Black pawn moves are generated on black turn", "[movegen]") {
    auto board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE7] = BlackPawn;
        return b;
    };
    State state(NoCastles, FileCNT, Black, 0);
    Position position(board(), state);
    MoveList moves = movegen::pseudo_legals(position);

    CHECK(moves.has({SquareE7, SquareE6, QuietMove}));
    CHECK(moves.has({SquareE7, SquareE5, DoublePawnPush}));
}

TEST_CASE("Movegen: White castling moves are generated when rights exist and path is clear", "[movegen]") {
    auto board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE1] = WhiteKing;
        b[SquareA1] = b[SquareH1] = WhiteRook;
        return b;
    };
    State state = State(WhiteCastles, FileCNT, White, 0);
    Position position(board(), state);
    MoveList moves = movegen::pseudo_legals(position);

    CHECK(moves.has({SquareE1, SquareG1, ShortCastle}));
    CHECK(moves.has({SquareE1, SquareC1, LongCastle}));
}

TEST_CASE("Movegen: Short castling is blocked by occupied F1 square", "[movegen]") {
    auto board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE1] = WhiteKing;
        b[SquareA1] = b[SquareH1] = WhiteRook;
        b[SquareF1] = WhiteKnight; // blocks short castle
        return b;
    };
    State state(WhiteCastles, FileCNT, White, 0);
    Position position(board(), state);
    MoveList moves = movegen::pseudo_legals(position);

    CHECK_FALSE(moves.has({SquareE1, SquareG1, ShortCastle}));
    CHECK(moves.has({SquareE1, SquareC1, LongCastle}));
}
