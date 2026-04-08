#include <catch2/catch_test_macros.hpp>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

#include "move_generation.hpp"
#include "piece_bitboards.hpp"
#include "position.hpp"
#include "state_info.hpp"
#include "utils.hpp"

TEST_CASE("Generate pseudo-legal moves from initial position", "[move_generation]") {
    ches::MoveList moves = generate_pseudo_legals(ches::Position::initial());
    CHECK(moves.size() == 20); // 16 pawn moves + 4 knight moves
}

TEST_CASE("Generate pseudo-legal moves from empty board", "[move_generation]") {
    ches::MoveList moves = generate_pseudo_legals(ches::Position{});
    CHECK(moves.size() == 0);
}

TEST_CASE("Generate pseudo-legal moves when only kings", "[move_generation]") {
    ches::PieceBitboards pieces{};
    pieces.set(ches::SquareE1, ches::WhiteKing);
    pieces.set(ches::SquareE8, ches::BlackKing);
    ches::Position pos(pieces, ches::StateInfo{});
    ches::MoveList moves = generate_pseudo_legals(pos);

    CHECK(moves.size() == 5); // white king should have 5 moves
}

TEST_CASE("Generate pseudo-legal moves when pawn promotion", "[move_generation]") {
    ches::PieceBitboards pieces{};
    pieces.set(ches::SquareE7, ches::WhitePawn);
    ches::Position pos(pieces, ches::StateInfo{});
    ches::MoveList moves = generate_pseudo_legals(pos);

    CHECK(moves.has({ches::SquareE7, ches::SquareE8, ches::QueenPromo}));
}

TEST_CASE("Initial position includes 8 double pawn pushes", "[move_generation]") {
    ches::MoveList moves = generate_pseudo_legals(ches::Position::initial());

    int double_pushes = 0;
    for (ches::Move move : moves) {
        if (move.flag() == ches::DoublePawnPush) {
            ++double_pushes;
        }
    }
    CHECK(double_pushes == 8);
}

TEST_CASE("Blocked pawn cannot push one or two squares", "[move_generation]") {
    ches::PieceBitboards pieces{};
    pieces.set(ches::SquareE2, ches::WhitePawn);
    pieces.set(ches::SquareE3, ches::BlackPawn); // blocker in front

    ches::Position pos(pieces, ches::StateInfo{});
    ches::MoveList moves = generate_pseudo_legals(pos);

    CHECK_FALSE(moves.has({ches::SquareE2, ches::SquareE3, ches::QuietMove}));
    CHECK_FALSE(moves.has({ches::SquareE2, ches::SquareE4, ches::DoublePawnPush}));
}

TEST_CASE("White pawn diagonal captures are generated", "[move_generation]") {
    ches::PieceBitboards pieces{};
    pieces.set(ches::SquareE5, ches::WhitePawn);
    pieces.set(ches::SquareD6, ches::BlackPawn);
    pieces.set(ches::SquareF6, ches::BlackPawn);

    ches::Position pos(pieces, ches::StateInfo{});
    ches::MoveList moves = generate_pseudo_legals(pos);

    CHECK(moves.has({ches::SquareE5, ches::SquareD6, ches::Capture}));
    CHECK(moves.has({ches::SquareE5, ches::SquareF6, ches::Capture}));
}

TEST_CASE("En passant capture is generated when en passant file is set", "[move_generation]") {
    ches::PieceBitboards pieces{};
    pieces.set(ches::SquareE5, ches::WhitePawn);
    pieces.set(ches::SquareD5, ches::BlackPawn);
    ches::StateInfo state{};
    state.set_en_passant(ches::FileD);

    ches::Position pos(pieces, state);
    ches::MoveList moves = generate_pseudo_legals(pos);

    CHECK(moves.has({ches::SquareE5, ches::SquareD6, ches::EnPassant}));
}

TEST_CASE("Black pawn moves are generated on black turn", "[move_generation]") {
    ches::PieceBitboards pieces{};
    pieces.set(ches::SquareE7, ches::BlackPawn);
    ches::StateInfo state{};
    state.switch_side(); // black to move

    ches::Position pos(pieces, state);
    ches::MoveList moves = generate_pseudo_legals(pos);

    CHECK(moves.has({ches::SquareE7, ches::SquareE6, ches::QuietMove}));
    CHECK(moves.has({ches::SquareE7, ches::SquareE5, ches::DoublePawnPush}));
}

TEST_CASE("White castling moves are generated when rights exist and path is clear", "[move_generation]") {
    ches::PieceBitboards pieces{};
    pieces.set(ches::SquareE1, ches::WhiteKing);
    pieces.set(ches::SquareA1, ches::WhiteRook);
    pieces.set(ches::SquareH1, ches::WhiteRook);

    ches::StateInfo state = ches::StateInfo(true, true, false, false, ches::FileCNT, ches::White, 0);
    ches::Position pos(pieces, state);
    ches::MoveList moves = generate_pseudo_legals(pos);

    CHECK(moves.has({ches::SquareE1, ches::SquareG1, ches::ShortCastle}));
    CHECK(moves.has({ches::SquareE1, ches::SquareC1, ches::LongCastle}));
}

TEST_CASE("Short castling is blocked by occupied F1 square", "[move_generation]") {
    ches::PieceBitboards pieces{};
    pieces.set(ches::SquareE1, ches::WhiteKing);
    pieces.set(ches::SquareA1, ches::WhiteRook);
    pieces.set(ches::SquareH1, ches::WhiteRook);
    pieces.set(ches::SquareF1, ches::WhiteKnight); // blocks short castle

    ches::StateInfo state(true, true, false, false, ches::FileCNT, ches::White, 0);
    ches::Position pos(pieces, state);
    ches::MoveList moves = generate_pseudo_legals(pos);

    CHECK_FALSE(moves.has({ches::SquareE1, ches::SquareG1, ches::ShortCastle}));
    CHECK(moves.has({ches::SquareE1, ches::SquareC1, ches::LongCastle}));
}
