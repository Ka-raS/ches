#include <array>
#include <bit>

#include <catch2/catch_test_macros.hpp>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

#include "pieces.hpp"
#include "position.hpp"
#include "state.hpp"
#include "utils.hpp"

using namespace cheslib;

namespace {

int count_piece(const std::array<Piece, SquareCNT> &board, Piece piece) {
    int count = 0;
    for (Piece p : board) {
        if (p == piece) {
            ++count;
        }
    }
    return count;
}

void check_consistency(const Position &pos) {
    const Pieces &pieces = pos.pieces();
    const std::array<Piece, SquareCNT> &board = pieces.board();

    Bitboard all = 0;
    Bitboard white = 0;
    Bitboard black = 0;

    for (Piece piece = Piece(0); piece < PieceCNT; ++piece) {
        const Bitboard bb = pieces.get(piece);
        CHECK(pieces.count(piece) == count_piece(board, piece));

        all |= bb;
        if (utils::side_of(piece) == White) {
            white |= bb;
        } else {
            black |= bb;
        }
    }

    CHECK(all == pieces.all());
    CHECK(white == pieces.all<White>());
    CHECK(black == pieces.all<Black>());
    CHECK((white & black) == 0);

    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        const Piece board_piece = board[sq];
        const bool occupied_on_board = board_piece < PieceCNT;

        CHECK(utils::has_square(all, sq) == occupied_on_board);
        if (occupied_on_board) {
            CHECK(utils::has_square(pieces.get(board_piece), sq));
        }
    }
}

void check_equality(const Position &pos1, const Position &pos2) {
    CHECK(pos1.state() == pos2.state());
    CHECK(pos1.pieces().board() == pos2.pieces().board());
    CHECK(pos1.key() == pos2.key());
}

} // namespace

TEST_CASE("Position: Quiet move keeps full consistency", "[position]") {
    const Position pos_init = Position::initial();
    Position pos = pos_init;

    {
        pos.do_move({SquareG1, SquareF3, QuietMove});
        const State state = pos.state();
        const std::array<Piece, SquareCNT> &board = pos.pieces().board();

        CHECK(board[SquareG1] == PieceCNT);
        CHECK(board[SquareF3] == WhiteKnight);
        CHECK(state.side_to_move() == Black);
        CHECK(state.en_passant() == FileCNT);
        CHECK(state.rule50_count() == 1);
        check_consistency(pos);
    }

    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Double pawn push updates en passant", "[position]") {
    const Position pos_init = Position::initial();
    Position pos = pos_init;

    {
        pos.do_move({SquareE2, SquareE4, DoublePawnPush});
        const State state = pos.state();
        const std::array<Piece, SquareCNT> &board = pos.pieces().board();

        CHECK(board[SquareE2] == PieceCNT);
        CHECK(board[SquareE4] == WhitePawn);
        CHECK(state.side_to_move() == Black);
        CHECK(state.en_passant() == FileE);
        CHECK(state.rule50_count() == 0);
        check_consistency(pos);
    }

    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Capture restores captured piece", "[position]") {
    const Position pos_init = []() {
        Pieces pieces{};
        pieces.put<White>(SquareE1, WhiteKing);
        pieces.put<Black>(SquareE8, BlackKing);
        pieces.put<White>(SquareA1, WhiteRook);
        pieces.put<Black>(SquareA8, BlackKnight);

        return Position(std::move(pieces), State(NoCastles, FileCNT, White, 7));
    }();

    Position pos = pos_init;
    {
        pos.do_move({SquareA1, SquareA8, Capture});
        const State state = pos.state();
        const Pieces &pieces = pos.pieces();
        const std::array<Piece, SquareCNT> &board = pieces.board();

        CHECK(board[SquareA1] == PieceCNT);
        CHECK(board[SquareA8] == WhiteRook);
        CHECK(pieces.count(BlackKnight) == 0);
        CHECK(state.side_to_move() == Black);
        CHECK(state.rule50_count() == 0);
        check_consistency(pos);
    }

    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: En passant is reversible", "[position]") {
    const Position pos_init = []() {
        Pieces pieces{};
        pieces.put<White>(SquareE1, WhiteKing);
        pieces.put<Black>(SquareE8, BlackKing);
        pieces.put<White>(SquareE5, WhitePawn);
        pieces.put<Black>(SquareD5, BlackPawn);

        return Position(std::move(pieces), State(NoCastles, FileCNT, White, 12));
    }();

    Position pos = pos_init;
    {
        pos.do_move({SquareE5, SquareD6, EnPassant});
        const State state = pos.state();
        const std::array<Piece, SquareCNT> &board = pos.pieces().board();

        CHECK(board[SquareE5] == PieceCNT);
        CHECK(board[SquareD5] == PieceCNT);
        CHECK(board[SquareD6] == WhitePawn);
        CHECK(state.side_to_move() == Black);
        CHECK(state.en_passant() == FileCNT);
        CHECK(state.rule50_count() == 0);
        check_consistency(pos);
    }

    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Short castle moves king and rook", "[position]") {
    const Position pos_init = []() {
        Pieces pieces{};
        pieces.put<White>(SquareE1, WhiteKing);
        pieces.put<White>(SquareH1, WhiteRook);
        pieces.put<Black>(SquareE8, BlackKing);

        return Position(std::move(pieces), State(WhiteShortCastles, FileCNT, White, 3));
    }();

    Position pos = pos_init;
    {
        pos.do_move({SquareE1, SquareG1, ShortCastle});
        const State state = pos.state();
        const std::array<Piece, SquareCNT> &board = pos.pieces().board();

        CHECK(board[SquareE1] == PieceCNT);
        CHECK(board[SquareH1] == PieceCNT);
        CHECK(board[SquareG1] == WhiteKing);
        CHECK(board[SquareF1] == WhiteRook);
        CHECK(state.side_to_move() == Black);
        CHECK(state.rule50_count() == 4);
        CHECK_FALSE(state.can_castles(WhiteShortCastles));
        check_consistency(pos);
    }

    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Promotion capture is reversible", "[position]") {
    const Position pos_init = []() {
        Pieces pieces{};
        pieces.put<White>(SquareE1, WhiteKing);
        pieces.put<Black>(SquareE8, BlackKing);
        pieces.put<White>(SquareA7, WhitePawn);
        pieces.put<Black>(SquareB8, BlackRook);

        return Position(std::move(pieces), State(NoCastles, FileCNT, White, 25));
    }();

    Position pos = pos_init;
    {
        pos.do_move({SquareA7, SquareB8, QueenPromoCap});
        const State state = pos.state();
        const Pieces &pieces = pos.pieces();
        const std::array<Piece, SquareCNT> &board = pieces.board();

        CHECK(board[SquareA7] == PieceCNT);
        CHECK(board[SquareB8] == WhiteQueen);
        CHECK(pieces.count(BlackRook) == 0);
        CHECK(pieces.count(WhitePawn) == 0);
        CHECK(pieces.count(WhiteQueen) == 1);
        CHECK(state.side_to_move() == Black);
        CHECK(state.rule50_count() == 0);
        check_consistency(pos);
    }

    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Castling rights updated", "[position]") {
    const Position pos_init = []() {
        Pieces pieces{};
        pieces.put<White>(SquareE1, WhiteKing);
        pieces.put<White>(SquareA1, WhiteRook);
        pieces.put<White>(SquareH1, WhiteRook);
        pieces.put<Black>(SquareE8, BlackKing);

        return Position(std::move(pieces), State(WhiteCastles, FileCNT, White, 0));
    }();

    Position pos = pos_init;
    {
        pos.do_move({SquareA1, SquareA2, QuietMove});
        CHECK_FALSE(pos.state().can_castles(WhiteLongCastles));
        CHECK(pos.state().can_castles(WhiteShortCastles));

        pos.undo_move();
        check_equality(pos, pos_init);
    }
    {
        pos.do_move({SquareE1, SquareE2, QuietMove});
        CHECK_FALSE(pos.state().can_castles(WhiteLongCastles));
        CHECK_FALSE(pos.state().can_castles(WhiteShortCastles));

        pos.undo_move();
        check_equality(pos, pos_init);
    }
}

TEST_CASE("Position: Multiple dos then undos", "[position]") {
    const Position pos_init = Position::initial();
    Position pos = pos_init;

    pos.do_move({SquareE2, SquareE4, DoublePawnPush});
    pos.do_move({SquareA7, SquareA6, QuietMove});
    pos.do_move({SquareG1, SquareF3, QuietMove});
    pos.do_move({SquareB8, SquareC6, QuietMove});

    check_consistency(pos);

    pos.undo_move();
    pos.undo_move();
    pos.undo_move();
    pos.undo_move();

    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Capturing rook revokes castling right", "[position]") {
    const Position pos_init = []() {
        Pieces pieces{};
        pieces.put<White>(SquareE1, WhiteKing);
        pieces.put<White>(SquareH1, WhiteRook);
        pieces.put<Black>(SquareE8, BlackKing);
        pieces.put<Black>(SquareH4, BlackQueen);

        return Position(std::move(pieces), State(BothCastles, FileCNT, Black, 4));
    }();

    Position pos = pos_init;
    {
        pos.do_move({SquareH4, SquareH1, Capture});
        const State state = pos.state();
        const std::array<Piece, SquareCNT> &board = pos.pieces().board();

        CHECK(board[SquareH1] == BlackQueen);
        CHECK(state.side_to_move() == White);
        CHECK_FALSE(state.can_castles(WhiteShortCastles));
        check_consistency(pos);
    }

    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}
