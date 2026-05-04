#include <catch2/catch_test_macros.hpp>

#include "movegen.hpp"

using namespace cheslib;

namespace {

const Position InitPosition = Position::initial();
const State EmptyState = State(NoCastles, FileCNT, White, 0);

const Pieces NoPieces = []() {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);
    return board;
}();

std::size_t count(const MoveList &moves) {
    return moves.end() - moves.begin();
}

} // namespace

TEST_CASE("Movegen: Generate pseudo-legal moves from initial position", "[movegen]") {
    MoveList moves = movegen::pseudo_legals(InitPosition);
    CHECK(count(moves) == 20); // 16 pawn moves + 4 knight moves
}

TEST_CASE("Movegen: Generate pseudo-legal moves from empty board", "[movegen]") {
    Pieces pieces = NoPieces;
    MoveList moves = movegen::pseudo_legals({std::move(pieces), EmptyState});
    CHECK(count(moves) == 0);
}

TEST_CASE("Movegen: Generate pseudo-legal moves when only kings", "[movegen]") {
    Pieces pieces = NoPieces;
    pieces.put(SquareE1, WhiteKing);
    pieces.put(SquareE8, BlackKing);

    MoveList moves = movegen::pseudo_legals({std::move(pieces), EmptyState});

    CHECK(count(moves) == 5); // white king should have 5 moves
}

TEST_CASE("Movegen: Generate pseudo-legal moves when pawn promotion", "[movegen]") {
    Pieces pieces = NoPieces;
    pieces.put(SquareE7, WhitePawn);

    MoveList moves = movegen::pseudo_legals({std::move(pieces), EmptyState});

    CHECK(moves.has({SquareE7, SquareE8, QueenPromo}));
}

TEST_CASE("Movegen: Initial position includes 8 double pawn pushes", "[movegen]") {
    MoveList moves = movegen::pseudo_legals(InitPosition);

    int double_pushes = 0;
    for (Move move : moves) {
        if (move.flag() == DoublePawnPush) {
            ++double_pushes;
        }
    }
    CHECK(double_pushes == 8);
}

TEST_CASE("Movegen: Blocked pawn cannot push one or two squares", "[movegen]") {
    Pieces pieces = NoPieces;
    pieces.put(SquareE2, WhitePawn);
    pieces.put(SquareE3, BlackPawn); // blocker in front

    MoveList moves = movegen::pseudo_legals({std::move(pieces), EmptyState});

    CHECK_FALSE(moves.has({SquareE2, SquareE3, QuietMove}));
    CHECK_FALSE(moves.has({SquareE2, SquareE4, DoublePawnPush}));
}

TEST_CASE("Movegen: White pawn diagonal captures are generated", "[movegen]") {
    Pieces pieces = NoPieces;
    pieces.put(SquareE5, WhitePawn);
    pieces.put(SquareD6, BlackPawn);
    pieces.put(SquareF6, BlackPawn);

    MoveList moves = movegen::pseudo_legals({std::move(pieces), EmptyState});

    CHECK(moves.has({SquareE5, SquareD6, Capture}));
    CHECK(moves.has({SquareE5, SquareF6, Capture}));
}

TEST_CASE("Movegen: En passant capture is generated when en passant file is set", "[movegen]") {
    Pieces pieces = NoPieces;
    pieces.put(SquareE5, WhitePawn);
    pieces.put(SquareD5, BlackPawn);

    State state(NoCastles, FileD, White, 0);
    MoveList moves = movegen::pseudo_legals({std::move(pieces), state});

    CHECK(moves.has({SquareE5, SquareD6, EnPassant}));
}

TEST_CASE("Movegen: Black pawn moves are generated on black turn", "[movegen]") {
    Pieces pieces = NoPieces;
    pieces.put(SquareE7, BlackPawn);

    State state(NoCastles, FileCNT, Black, 0);
    MoveList moves = movegen::pseudo_legals({std::move(pieces), state});

    CHECK(moves.has({SquareE7, SquareE6, QuietMove}));
    CHECK(moves.has({SquareE7, SquareE5, DoublePawnPush}));
}

TEST_CASE("Movegen: White castling moves are generated when rights exist and path is clear", "[movegen]") {
    Pieces pieces = NoPieces;
    pieces.put(SquareE1, WhiteKing);
    pieces.put(SquareA1, WhiteRook);
    pieces.put(SquareH1, WhiteRook);

    State state = State(WhiteCastles, FileCNT, White, 0);
    MoveList moves = movegen::pseudo_legals({std::move(pieces), state});

    CHECK(moves.has({SquareE1, SquareG1, ShortCastle}));
    CHECK(moves.has({SquareE1, SquareC1, LongCastle}));
}

TEST_CASE("Movegen: Short castling is blocked by occupied F1 square", "[movegen]") {
    Pieces pieces = NoPieces;
    pieces.put(SquareE1, WhiteKing);
    pieces.put(SquareA1, WhiteRook);
    pieces.put(SquareH1, WhiteRook);
    pieces.put(SquareF1, WhiteKnight); // blocks short castle

    State state(WhiteCastles, FileCNT, White, 0);
    MoveList moves = movegen::pseudo_legals({std::move(pieces), state});

    CHECK_FALSE(moves.has({SquareE1, SquareG1, ShortCastle}));
    CHECK(moves.has({SquareE1, SquareC1, LongCastle}));
}
