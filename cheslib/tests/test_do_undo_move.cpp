#include <array>
#include <bit>

#include <catch2/catch_test_macros.hpp>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

#include "piece_bitboards.hpp"
#include "position.hpp"
#include "state.hpp"
#include "utils.hpp"

using namespace ches;

namespace {

int count_piece(const Position &pos, Piece piece) {
    int count = 0;
    for (Piece p : pos.board()) {
        if (p == piece) {
            ++count;
        }
    }
    return count;
}

void check_bitboards_consistency(const Position &pos) {
    const std::array<Piece, SquareCNT> &board = pos.board();
    const PieceBitboards &pieces = pos.pieces();

    Bitboard all = 0;
    Bitboard white = 0;
    Bitboard black = 0;

    for (Piece piece = WhitePawn; piece < PieceCNT; ++piece) {
        const Bitboard bb = pieces.get(piece);
        CHECK(std::popcount(bb) == count_piece(pos, piece));

        all |= bb;
        if (side_of(piece) == White) {
            white |= bb;
        } else {
            black |= bb;
        }
    }

    CHECK(all == pieces.all());
    CHECK(white == pieces.all<White>());
    CHECK(black == pieces.all<Black>());
    CHECK((white & black) == 0);

    for (Square sq = SquareA1; sq < SquareCNT; ++sq) {
        const Piece board_piece = board[sq];
        const bool occupied_on_board = board_piece < PieceCNT;

        CHECK(has_square(all, sq) == occupied_on_board);
        if (occupied_on_board) {
            CHECK(has_square(pieces.get(board_piece), sq));
        }
    }
}

} // namespace

TEST_CASE("Do/Undo move: Quiet move keeps full consistency", "[do_undo]") {
    Position pos = Position::initial();
    const auto bound_init = pos.board();
    const State state_init = pos.state();

    pos.do_move(Move(SquareG1, SquareF3, QuietMove));
    CHECK(pos.board()[SquareG1] == PieceCNT);
    CHECK(pos.board()[SquareF3] == WhiteKnight);
    CHECK(pos.state().side_to_move() == Black);
    CHECK(pos.state().en_passant() == FileCNT);
    CHECK(pos.state().rule50_count() == state_init.rule50_count() + 1);
    check_bitboards_consistency(pos);

    pos.undo_move();
    CHECK(pos.board() == bound_init);
    CHECK(pos.state() == state_init);
    check_bitboards_consistency(pos);
}

TEST_CASE("Do/Undo move: Double pawn push updates en passant", "[do_undo]") {
    Position pos = Position::initial();
    const auto bound_init = pos.board();
    const State state_init = pos.state();

    pos.do_move(Move(SquareE2, SquareE4, DoublePawnPush));
    CHECK(pos.board()[SquareE2] == PieceCNT);
    CHECK(pos.board()[SquareE4] == WhitePawn);
    CHECK(pos.state().side_to_move() == Black);
    CHECK(pos.state().en_passant() == FileE);
    CHECK(pos.state().rule50_count() == 0);
    check_bitboards_consistency(pos);

    pos.undo_move();
    CHECK(pos.board() == bound_init);
    CHECK(pos.state() == state_init);
    check_bitboards_consistency(pos);
}

TEST_CASE("Do/Undo move: Capture restores captured piece", "[do_undo]") {
    PieceBitboards pieces{};
    pieces.set(SquareE1, WhiteKing);
    pieces.set(SquareE8, BlackKing);
    pieces.set(SquareA1, WhiteRook);
    pieces.set(SquareA8, BlackKnight);

    Position pos(pieces, State(NoCastles, FileCNT, White, 7));
    const auto bound_init = pos.board();
    const State state_init = pos.state();

    pos.do_move(Move(SquareA1, SquareA8, Capture));
    CHECK(pos.board()[SquareA1] == PieceCNT);
    CHECK(pos.board()[SquareA8] == WhiteRook);
    CHECK(pos.count(BlackKnight) == 0);
    CHECK(pos.state().side_to_move() == Black);
    CHECK(pos.state().rule50_count() == 0);
    check_bitboards_consistency(pos);

    pos.undo_move();
    CHECK(pos.board() == bound_init);
    CHECK(pos.state() == state_init);
    CHECK(pos.count(BlackKnight) == 1);
    check_bitboards_consistency(pos);
}

TEST_CASE("Do/Undo move: En passant is reversible", "[do_undo]") {
    PieceBitboards pieces{};
    pieces.set(SquareE1, WhiteKing);
    pieces.set(SquareE8, BlackKing);
    pieces.set(SquareE5, WhitePawn);
    pieces.set(SquareD5, BlackPawn);

    Position pos(pieces, State(NoCastles, FileD, White, 12));
    const auto bound_init = pos.board();
    const State state_init = pos.state();

    pos.do_move(Move(SquareE5, SquareD6, EnPassant));
    CHECK(pos.board()[SquareE5] == PieceCNT);
    CHECK(pos.board()[SquareD5] == PieceCNT);
    CHECK(pos.board()[SquareD6] == WhitePawn);
    CHECK(pos.state().side_to_move() == Black);
    CHECK(pos.state().en_passant() == FileCNT);
    CHECK(pos.state().rule50_count() == 0);
    check_bitboards_consistency(pos);

    pos.undo_move();
    CHECK(pos.board() == bound_init);
    CHECK(pos.state() == state_init);
    check_bitboards_consistency(pos);
}

TEST_CASE("Do/Undo move: Short castle moves king and rook", "[do_undo]") {
    PieceBitboards pieces{};
    pieces.set(SquareE1, WhiteKing);
    pieces.set(SquareH1, WhiteRook);
    pieces.set(SquareE8, BlackKing);

    Position pos(pieces, State(WhiteShortCastles, FileCNT, White, 3));
    const auto bound_init = pos.board();
    const State state_init = pos.state();

    pos.do_move(Move(SquareE1, SquareG1, ShortCastle));
    CHECK(pos.board()[SquareE1] == PieceCNT);
    CHECK(pos.board()[SquareH1] == PieceCNT);
    CHECK(pos.board()[SquareG1] == WhiteKing);
    CHECK(pos.board()[SquareF1] == WhiteRook);
    CHECK(pos.state().side_to_move() == Black);
    CHECK_FALSE(pos.state().can_castles(WhiteShortCastles));
    CHECK(pos.state().rule50_count() == state_init.rule50_count() + 1);
    check_bitboards_consistency(pos);

    pos.undo_move();
    CHECK(pos.board() == bound_init);
    CHECK(pos.state() == state_init);
    check_bitboards_consistency(pos);
}

TEST_CASE("Do/Undo move: Promotion capture is reversible", "[do_undo]") {
    PieceBitboards pieces{};
    pieces.set(SquareE1, WhiteKing);
    pieces.set(SquareE8, BlackKing);
    pieces.set(SquareA7, WhitePawn);
    pieces.set(SquareB8, BlackRook);

    Position pos(pieces, State(NoCastles, FileCNT, White, 25));
    const auto bound_init = pos.board();
    const State state_init = pos.state();

    pos.do_move(Move(SquareA7, SquareB8, QueenPromoCap));
    CHECK(pos.board()[SquareA7] == PieceCNT);
    CHECK(pos.board()[SquareB8] == WhiteQueen);
    CHECK(pos.count(BlackRook) == 0);
    CHECK(pos.count(WhitePawn) == 0);
    CHECK(pos.count(WhiteQueen) == 1);
    CHECK(pos.state().side_to_move() == Black);
    CHECK(pos.state().rule50_count() == 0);
    check_bitboards_consistency(pos);

    pos.undo_move();
    CHECK(pos.board() == bound_init);
    CHECK(pos.state() == state_init);
    CHECK(pos.count(BlackRook) == 1);
    CHECK(pos.count(WhitePawn) == 1);
    CHECK(pos.count(WhiteQueen) == 0);
    check_bitboards_consistency(pos);
}

TEST_CASE("Do/Undo move: Castling rights updated", "[do_undo]") {
    PieceBitboards pieces{};
    pieces.set(SquareE1, WhiteKing);
    pieces.set(SquareA1, WhiteRook);
    pieces.set(SquareH1, WhiteRook);
    pieces.set(SquareE8, BlackKing);

    Position pos(pieces, State(WhiteCastles, FileCNT, White, 0));
    const State state_init = pos.state();

    pos.do_move(Move(SquareA1, SquareA2, QuietMove));
    CHECK_FALSE(pos.state().can_castles(WhiteLongCastles));
    CHECK(pos.state().can_castles(WhiteShortCastles));
    pos.undo_move();
    CHECK(pos.state() == state_init);

    pos.do_move(Move(SquareE1, SquareE2, QuietMove));
    CHECK_FALSE(pos.state().can_castles(WhiteLongCastles));
    CHECK_FALSE(pos.state().can_castles(WhiteShortCastles));
    pos.undo_move();
    CHECK(pos.state() == state_init);
}

TEST_CASE("Do/Undo move: Multiple dos then undos", "[do_undo]") {
    Position pos = Position::initial();
    const auto bound_init = pos.board();
    const State state_init = pos.state();

    pos.do_move(Move(SquareE2, SquareE4, DoublePawnPush));
    pos.do_move(Move(SquareA7, SquareA6, QuietMove));
    pos.do_move(Move(SquareG1, SquareF3, QuietMove));
    pos.do_move(Move(SquareB8, SquareC6, QuietMove));

    check_bitboards_consistency(pos);

    pos.undo_move();
    pos.undo_move();
    pos.undo_move();
    pos.undo_move();

    CHECK(pos.board() == bound_init);
    CHECK(pos.state() == state_init);
    check_bitboards_consistency(pos);
}

TEST_CASE("Do/Undo move: Capturing rook revokes castling right", "[do_undo]") {
    PieceBitboards pieces{};
    pieces.set(SquareE1, WhiteKing);
    pieces.set(SquareH1, WhiteRook);
    pieces.set(SquareE8, BlackKing);
    pieces.set(SquareH4, BlackQueen);

    Position pos(pieces, State(BothCastles, FileCNT, Black, 4));
    const auto board_before = pos.board();
    const State state_before = pos.state();

    pos.do_move(Move(SquareH4, SquareH1, Capture));
    CHECK(pos.board()[SquareH1] == BlackQueen);
    CHECK_FALSE(pos.state().can_castles(WhiteShortCastles));
    CHECK(pos.state().side_to_move() == White);
    check_bitboards_consistency(pos);

    pos.undo_move();
    CHECK(pos.board() == board_before);
    CHECK(pos.state() == state_before);
    check_bitboards_consistency(pos);
}
