#include <catch2/catch_test_macros.hpp>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

#include "move_generation.hpp"
#include "piece_bitboards.hpp"
#include "position.hpp"
#include "state.hpp"
#include "utils.hpp"

using namespace ches;

TEST_CASE("Move generation: Generate pseudo-legal moves from initial position", "[move_generation]") {
    MoveList moves = generate_pseudo_legals(Position::initial());
    CHECK(moves.size() == 20); // 16 pawn moves + 4 knight moves
}

TEST_CASE("Move generation: Generate pseudo-legal moves from empty board", "[move_generation]") {
    MoveList moves = generate_pseudo_legals(Position{});
    CHECK(moves.size() == 0);
}

TEST_CASE("Move generation: Generate pseudo-legal moves when only kings", "[move_generation]") {
    PieceBitboards pieces{};
    pieces.set(SquareE1, WhiteKing);
    pieces.set(SquareE8, BlackKing);
    Position pos(pieces, State{});
    MoveList moves = generate_pseudo_legals(pos);

    CHECK(moves.size() == 5); // white king should have 5 moves
}

TEST_CASE("Move generation: Generate pseudo-legal moves when pawn promotion", "[move_generation]") {
    PieceBitboards pieces{};
    pieces.set(SquareE7, WhitePawn);
    Position pos(pieces, State{});
    MoveList moves = generate_pseudo_legals(pos);

    CHECK(moves.has({SquareE7, SquareE8, QueenPromo}));
}

TEST_CASE("Move generation: Initial position includes 8 double pawn pushes", "[move_generation]") {
    MoveList moves = generate_pseudo_legals(Position::initial());

    int double_pushes = 0;
    for (Move move : moves) {
        if (move.flag() == DoublePawnPush) {
            ++double_pushes;
        }
    }
    CHECK(double_pushes == 8);
}

TEST_CASE("Move generation: Blocked pawn cannot push one or two squares", "[move_generation]") {
    PieceBitboards pieces{};
    pieces.set(SquareE2, WhitePawn);
    pieces.set(SquareE3, BlackPawn); // blocker in front

    Position pos(pieces, State{});
    MoveList moves = generate_pseudo_legals(pos);

    CHECK_FALSE(moves.has({SquareE2, SquareE3, QuietMove}));
    CHECK_FALSE(moves.has({SquareE2, SquareE4, DoublePawnPush}));
}

TEST_CASE("Move generation: White pawn diagonal captures are generated", "[move_generation]") {
    PieceBitboards pieces{};
    pieces.set(SquareE5, WhitePawn);
    pieces.set(SquareD6, BlackPawn);
    pieces.set(SquareF6, BlackPawn);

    Position pos(pieces, State{});
    MoveList moves = generate_pseudo_legals(pos);

    CHECK(moves.has({SquareE5, SquareD6, Capture}));
    CHECK(moves.has({SquareE5, SquareF6, Capture}));
}

TEST_CASE("Move generation: En passant capture is generated when en passant file is set", "[move_generation]") {
    PieceBitboards pieces{};
    pieces.set(SquareE5, WhitePawn);
    pieces.set(SquareD5, BlackPawn);
    State state{};
    state.set_en_passant(FileD);

    Position pos(pieces, state);
    MoveList moves = generate_pseudo_legals(pos);

    CHECK(moves.has({SquareE5, SquareD6, EnPassant}));
}

TEST_CASE("Move generation: Black pawn moves are generated on black turn", "[move_generation]") {
    PieceBitboards pieces{};
    pieces.set(SquareE7, BlackPawn);
    State state{};
    state.switch_side(); // black to move

    Position pos(pieces, state);
    MoveList moves = generate_pseudo_legals(pos);

    CHECK(moves.has({SquareE7, SquareE6, QuietMove}));
    CHECK(moves.has({SquareE7, SquareE5, DoublePawnPush}));
}

TEST_CASE("Move generation: White castling moves are generated when rights exist and path is clear", "[move_generation]") {
    PieceBitboards pieces{};
    pieces.set(SquareE1, WhiteKing);
    pieces.set(SquareA1, WhiteRook);
    pieces.set(SquareH1, WhiteRook);

    State state = State(WhiteCastles, FileCNT, White, 0);
    Position pos(pieces, state);
    MoveList moves = generate_pseudo_legals(pos);

    CHECK(moves.has({SquareE1, SquareG1, ShortCastle}));
    CHECK(moves.has({SquareE1, SquareC1, LongCastle}));
}

TEST_CASE("Move generation: Short castling is blocked by occupied F1 square", "[move_generation]") {
    PieceBitboards pieces{};
    pieces.set(SquareE1, WhiteKing);
    pieces.set(SquareA1, WhiteRook);
    pieces.set(SquareH1, WhiteRook);
    pieces.set(SquareF1, WhiteKnight); // blocks short castle

    State state(WhiteCastles, FileCNT, White, 0);
    Position pos(pieces, state);
    MoveList moves = generate_pseudo_legals(pos);

    CHECK_FALSE(moves.has({SquareE1, SquareG1, ShortCastle}));
    CHECK(moves.has({SquareE1, SquareC1, LongCastle}));
}
