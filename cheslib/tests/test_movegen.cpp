#include <algorithm>

#include <catch2/catch_test_macros.hpp>

#include "movegen.hpp"

using namespace cheslib;

namespace {

bool has_move(const Array<MoveScore, 256> &moves, Move move) {
    return std::any_of(moves.begin(), moves.end(), [&](MoveScore entry) {
        return entry.move == move;
    });
}

} // namespace

TEST_CASE("Movegen: Generate pseudo-legal moves from initial position", "[movegen]") {
    Array<MoveScore, 256> moves = movegen::pseudo_legals(Position::initial());

    CHECK(moves.size() == 20); // 16 pawn moves + 4 knight moves
}

TEST_CASE("Movegen: Generate pseudo-legal moves from empty board", "[movegen]") {
    auto board = [] {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        return b;
    };
    PositionState state(NoCastles, FileCNT, White, 0);
    Position position(board(), state);
    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(moves.size() == 0);
}

TEST_CASE("Movegen: Generate pseudo-legal moves when only kings", "[movegen]") {
    auto board = [] {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE1] = WhiteKing;
        b[SquareE8] = BlackKing;
        return b;
    };
    PositionState state(NoCastles, FileCNT, White, 0);
    Position position(board(), state);
    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(moves.size() == 5); // white king should have 5 moves
}

TEST_CASE("Movegen: Generate pseudo-legal moves when pawn promotion", "[movegen]") {
    auto board = [] {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE7] = WhitePawn;
        return b;
    };
    PositionState state(NoCastles, FileCNT, White, 0);
    Position position(board(), state);
    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    auto target = std::find_if(moves.begin(), moves.end(), [](const MoveScore &ms) {
        return ms.move == Move(SquareE7, SquareE8, QueenPromo);
    });

    CHECK(has_move(moves, Move(SquareE7, SquareE8, QueenPromo)));
}

TEST_CASE("Movegen: Initial position includes 8 double pawn pushes", "[movegen]") {
    Array<MoveScore, 256> moves = movegen::pseudo_legals(Position::initial());

    int double_pushes = 0;
    for (auto [move, _] : moves) {
        if (move.flag() == DoublePawnPush) {
            ++double_pushes;
        }
    }
    CHECK(double_pushes == 8);
}

TEST_CASE("Movegen: Blocked pawn cannot push one or two squares", "[movegen]") {
    auto board = [] {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE2] = WhitePawn;
        b[SquareE3] = BlackPawn; // blocker in front
        return b;
    };
    PositionState state(NoCastles, FileCNT, White, 0);
    Position position(board(), state);
    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK_FALSE(has_move(moves, Move(SquareE2, SquareE3, QuietMove)));
    CHECK_FALSE(has_move(moves, Move(SquareE2, SquareE4, DoublePawnPush)));
}

TEST_CASE("Movegen: White pawn diagonal captures are generated", "[movegen]") {
    auto board = [] {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE5] = WhitePawn;
        b[SquareD6] = b[SquareF6] = BlackPawn;
        return b;
    };
    PositionState state(NoCastles, FileCNT, White, 0);
    Position position(board(), state);
    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(has_move(moves, Move(SquareE5, SquareD6, Capture)));
    CHECK(has_move(moves, Move(SquareE5, SquareF6, Capture)));
}

TEST_CASE("Movegen: En passant capture is generated when en passant file is set", "[movegen]") {
    auto board = [] {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE5] = WhitePawn;
        b[SquareD5] = BlackPawn;
        return b;
    };
    PositionState state(NoCastles, FileD, White, 0);
    Position position(board(), state);
    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(has_move(moves, Move(SquareE5, SquareD6, EnPassant)));
}

TEST_CASE("Movegen: Black pawn moves are generated on black turn", "[movegen]") {
    auto board = [] {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE7] = BlackPawn;
        return b;
    };
    PositionState state(NoCastles, FileCNT, Black, 0);
    Position position(board(), state);
    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(has_move(moves, Move(SquareE7, SquareE6, QuietMove)));
    CHECK(has_move(moves, Move(SquareE7, SquareE5, DoublePawnPush)));
}

TEST_CASE("Movegen: White castling moves are generated when rights exist and path is clear", "[movegen]") {
    auto board = [] {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE1] = WhiteKing;
        b[SquareA1] = b[SquareH1] = WhiteRook;
        return b;
    };
    PositionState state = PositionState(WhiteCastles, FileCNT, White, 0);
    Position position(board(), state);
    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(has_move(moves, Move(SquareE1, SquareG1, ShortCastle)));
    CHECK(has_move(moves, Move(SquareE1, SquareC1, LongCastle)));
}

TEST_CASE("Movegen: Short castling is blocked by occupied F1 square", "[movegen]") {
    auto board = [] {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE1] = WhiteKing;
        b[SquareA1] = b[SquareH1] = WhiteRook;
        b[SquareF1] = WhiteKnight; // blocks short castle
        return b;
    };
    PositionState state(WhiteCastles, FileCNT, White, 0);
    Position position(board(), state);
    Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK_FALSE(has_move(moves, Move(SquareE1, SquareG1, ShortCastle)));
    CHECK(has_move(moves, Move(SquareE1, SquareC1, LongCastle)));
}
