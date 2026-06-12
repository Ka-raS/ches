#include "movegen.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace cheslib;

namespace {

bool has_move(const Array<MoveScore, 256> &moves, const Move target) {
    for (const auto [move, _] : moves) {
        if (move == target) {
            return true;
        }
    }
    return false;
}

} // namespace

TEST_CASE("Movegen: Generate pseudo-legal moves from initial position", "[movegen]") {
    const Array<MoveScore, 256> moves = movegen::pseudo_legals(Position::initial());

    CHECK(moves.size() == 20); // 16 pawn moves + 4 knight moves
}

TEST_CASE("Movegen: Generate pseudo-legal moves from empty board", "[movegen]") {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);

    const PositionState state{NoCastles, FileCNT, White, 0};
    const Position position{state, board};
    const Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(moves.size() == 0);
}

TEST_CASE("Movegen: Generate pseudo-legal moves when only kings", "[movegen]") {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);
    board[SquareE1] = WhiteKing;
    board[SquareE8] = BlackKing;

    const PositionState state{NoCastles, FileCNT, White, 0};
    const Position position{state, board};
    const Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(moves.size() == 5); // white king should have 5 moves
}

TEST_CASE("Movegen: Generate pseudo-legal moves when pawn promotion", "[movegen]") {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);
    board[SquareE7] = WhitePawn;

    const PositionState state{NoCastles, FileCNT, White, 0};
    const Position position{state, board};
    const Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(has_move(moves, Move{SquareE7, SquareE8, QueenPromo}));
}

TEST_CASE("Movegen: Initial position includes 8 double pawn pushes", "[movegen]") {
    const Array<MoveScore, 256> moves = movegen::pseudo_legals(Position::initial());

    int double_pushes = 0;
    for (const auto [move, _] : moves) {
        if (move.flag() == DoublePawnPush) {
            ++double_pushes;
        }
    }
    CHECK(double_pushes == 8);
}

TEST_CASE("Movegen: Blocked pawn cannot push one or two squares", "[movegen]") {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);
    board[SquareE2] = WhitePawn;
    board[SquareE3] = BlackPawn; // blocker in front

    const PositionState state{NoCastles, FileCNT, White, 0};
    const Position position{state, board};
    const Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK_FALSE(has_move(moves, Move{SquareE2, SquareE3, QuietMove}));
    CHECK_FALSE(has_move(moves, Move{SquareE2, SquareE4, DoublePawnPush}));
}

TEST_CASE("Movegen: White pawn diagonal captures are generated", "[movegen]") {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);
    board[SquareE5] = WhitePawn;
    board[SquareD6] = board[SquareF6] = BlackPawn;

    const PositionState state{NoCastles, FileCNT, White, 0};
    const Position position{state, board};
    const Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(has_move(moves, Move{SquareE5, SquareD6, Capture}));
    CHECK(has_move(moves, Move{SquareE5, SquareF6, Capture}));
}

TEST_CASE("Movegen: En passant capture is generated when en passant file is set", "[movegen]") {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);
    board[SquareE5] = WhitePawn;
    board[SquareD5] = BlackPawn;

    const PositionState state{NoCastles, FileD, White, 0};
    const Position position{state, board};
    const Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(has_move(moves, Move{SquareE5, SquareD6, EnPassant}));
}

TEST_CASE("Movegen: Black pawn moves are generated on black turn", "[movegen]") {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);
    board[SquareE7] = BlackPawn;

    const PositionState state{NoCastles, FileCNT, Black, 0};
    const Position position{state, board};
    const Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(has_move(moves, Move{SquareE7, SquareE6, QuietMove}));
    CHECK(has_move(moves, Move{SquareE7, SquareE5, DoublePawnPush}));
}

TEST_CASE("Movegen: White castling moves are generated when rights exist and path is clear", "[movegen]") {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);
    board[SquareE1] = WhiteKing;
    board[SquareA1] = board[SquareH1] = WhiteRook;

    const PositionState state{WhiteCastles, FileCNT, White, 0};
    const Position position{state, board};
    const Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK(has_move(moves, Move{SquareE1, SquareG1, ShortCastle}));
    CHECK(has_move(moves, Move{SquareE1, SquareC1, LongCastle}));
}

TEST_CASE("Movegen: Short castling is blocked by occupied F1 square", "[movegen]") {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);
    board[SquareE1] = WhiteKing;
    board[SquareA1] = board[SquareH1] = WhiteRook;
    board[SquareF1] = WhiteKnight; // blocks short castle

    const PositionState state{WhiteCastles, FileCNT, White, 0};
    const Position position{state, board};
    const Array<MoveScore, 256> moves = movegen::pseudo_legals(position);

    CHECK_FALSE(has_move(moves, Move{SquareE1, SquareG1, ShortCastle}));
    CHECK(has_move(moves, Move{SquareE1, SquareC1, LongCastle}));
}
