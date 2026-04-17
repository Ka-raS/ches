#include <catch2/catch_test_macros.hpp>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

#include "movegen.hpp"
#include "pieces.hpp"
#include "position.hpp"
#include "state.hpp"
#include "utils.hpp"

using namespace cheslib;

TEST_CASE("Movegen: Generate pseudo-legal moves from initial position", "[movegen]") {
    MoveList moves = movegen::pseudo_legals(Pieces::initial(), State::initial());
    CHECK(moves.size() == 20); // 16 pawn moves + 4 knight moves
}

TEST_CASE("Movegen: Generate pseudo-legal moves from empty board", "[movegen]") {
    MoveList moves = movegen::pseudo_legals(Pieces{}, State{});
    CHECK(moves.size() == 0);
}

TEST_CASE("Movegen: Generate pseudo-legal moves when only kings", "[movegen]") {
    Pieces pieces{};
    pieces.put<White>(SquareE1, WhiteKing);
    pieces.put<Black>(SquareE8, BlackKing);
    
    MoveList moves = movegen::pseudo_legals(pieces, State{});

    CHECK(moves.size() == 5); // white king should have 5 moves
}

TEST_CASE("Movegen: Generate pseudo-legal moves when pawn promotion", "[movegen]") {
    Pieces pieces{};
    pieces.put<White>(SquareE7, WhitePawn);

    MoveList moves = movegen::pseudo_legals(pieces, State{});

    CHECK(moves.has({SquareE7, SquareE8, QueenPromo}));
}

TEST_CASE("Movegen: Initial position includes 8 double pawn pushes", "[movegen]") {
    MoveList moves = movegen::pseudo_legals(Pieces::initial(), State::initial());

    int double_pushes = 0;
    for (Move move : moves) {
        if (move.flag() == DoublePawnPush) {
            ++double_pushes;
        }
    }
    CHECK(double_pushes == 8);
}

TEST_CASE("Movegen: Blocked pawn cannot push one or two squares", "[movegen]") {
    Pieces pieces{};
    pieces.put<White>(SquareE2, WhitePawn);
    pieces.put<Black>(SquareE3, BlackPawn); // blocker in front

    MoveList moves = movegen::pseudo_legals(pieces, State{});

    CHECK_FALSE(moves.has({SquareE2, SquareE3, QuietMove}));
    CHECK_FALSE(moves.has({SquareE2, SquareE4, DoublePawnPush}));
}

TEST_CASE("Movegen: White pawn diagonal captures are generated", "[movegen]") {
    Pieces pieces{};
    pieces.put<White>(SquareE5, WhitePawn);
    pieces.put<Black>(SquareD6, BlackPawn);
    pieces.put<Black>(SquareF6, BlackPawn);

    MoveList moves = movegen::pseudo_legals(pieces, State{});

    CHECK(moves.has({SquareE5, SquareD6, Capture}));
    CHECK(moves.has({SquareE5, SquareF6, Capture}));
}

TEST_CASE("Movegen: En passant capture is generated when en passant file is set", "[movegen]") {
    Pieces pieces{};
    pieces.put<White>(SquareE5, WhitePawn);
    pieces.put<Black>(SquareD5, BlackPawn);

    State state(NoCastles, FileD, White, 0);
    MoveList moves = movegen::pseudo_legals(pieces, state);

    CHECK(moves.has({SquareE5, SquareD6, EnPassant}));
}

TEST_CASE("Movegen: Black pawn moves are generated on black turn", "[movegen]") {
    Pieces pieces{};
    pieces.put<Black>(SquareE7, BlackPawn);

    State state(NoCastles, FileCNT, Black, 0);
    MoveList moves = movegen::pseudo_legals(pieces, state);

    CHECK(moves.has({SquareE7, SquareE6, QuietMove}));
    CHECK(moves.has({SquareE7, SquareE5, DoublePawnPush}));
}

TEST_CASE("Movegen: White castling moves are generated when rights exist and path is clear", "[movegen]") {
    Pieces pieces{};
    pieces.put<White>(SquareE1, WhiteKing);
    pieces.put<White>(SquareA1, WhiteRook);
    pieces.put<White>(SquareH1, WhiteRook);

    State state = State(WhiteCastles, FileCNT, White, 0);
    MoveList moves = movegen::pseudo_legals(pieces, state);

    CHECK(moves.has({SquareE1, SquareG1, ShortCastle}));
    CHECK(moves.has({SquareE1, SquareC1, LongCastle}));
}

TEST_CASE("Movegen: Short castling is blocked by occupied F1 square", "[movegen]") {
    Pieces pieces{};
    pieces.put<White>(SquareE1, WhiteKing);
    pieces.put<White>(SquareA1, WhiteRook);
    pieces.put<White>(SquareH1, WhiteRook);
    pieces.put<White>(SquareF1, WhiteKnight); // blocks short castle

    State state(WhiteCastles, FileCNT, White, 0);
    MoveList moves = movegen::pseudo_legals(pieces, state);

    CHECK_FALSE(moves.has({SquareE1, SquareG1, ShortCastle}));
    CHECK(moves.has({SquareE1, SquareC1, LongCastle}));
}
