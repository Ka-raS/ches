#include <catch2/catch_test_macros.hpp>

#include "position.hpp"

using namespace cheslib;

namespace {

void check_equality(const Position &pos1, const Position &pos2) {
    CHECK(pos1.state() == pos2.state());
    CHECK(pos1.pieces().board() == pos2.pieces().board());
    CHECK(pos1.key() == pos2.key());
}

void check_consistency(const Position &pos) {
    const Pieces &pieces = pos.pieces();
    const std::array<Piece, SquareCNT> &board = pieces.board();

    Bitboard all = 0;
    Bitboard white = 0;
    Bitboard black = 0;

    auto count = [&board](Piece piece) {
        int cnt = 0;
        for (Piece p : board) {
            if (p == piece) {
                ++cnt;
            }
        }
        return cnt;
    };

    for (Piece piece = Piece(0); piece < PieceCNT; ++piece) {
        Bitboard bb = pieces.get(piece);
        CHECK(count(piece) == std::popcount(bb));

        all |= bb;
        if (types::side_of(piece) == White) {
            white |= bb;
        } else {
            black |= bb;
        }
    }

    CHECK(all == pieces.all());
    CHECK(white == pieces.all_of(White));
    CHECK(black == pieces.all_of(Black));
    CHECK((white & black) == 0);

    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        const Piece piece = board[sq];
        const bool has_piece = piece < PieceCNT;

        CHECK(types::has_square(all, sq) == has_piece);
        if (has_piece) {
            Bitboard bb = pieces.get(piece);
            CHECK(types::has_square(bb, sq));
        }
    }
}

} // namespace

TEST_CASE("Position: Quiet move keeps full consistency", "[position]") {
    const Position pos_init = Position::initial();
    Position pos = pos_init;
    pos.do_move({SquareG1, SquareF3, QuietMove});

    const State state = pos.state();
    const std::array<Piece, SquareCNT> &board = pos.pieces().board();

    CHECK(board[SquareG1] == PieceCNT);
    CHECK(board[SquareF3] == WhiteKnight);
    CHECK(state.side_to_move() == Black);
    CHECK(state.en_passant() == FileCNT);
    CHECK(state.rule50_count() == 1);

    check_consistency(pos);
    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Double pawn push updates en passant", "[position]") {
    auto init_board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE2] = WhitePawn;
        b[SquareD4] = BlackPawn;
        return b;
    };

    const Position pos_init(init_board(), State(NoCastles, FileCNT, White, 0));
    Position pos = pos_init;
    pos.do_move({SquareE2, SquareE4, DoublePawnPush});

    const State state = pos.state();
    const std::array<Piece, SquareCNT> &board = pos.pieces().board();

    CHECK(board[SquareE2] == PieceCNT);
    CHECK(board[SquareE4] == WhitePawn);
    CHECK(state.side_to_move() == Black);
    CHECK(state.en_passant() == FileE);
    CHECK(state.rule50_count() == 0);

    check_consistency(pos);
    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Capture restores captured piece", "[position]") {
    auto init_board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE1] = WhiteKing;
        b[SquareE8] = BlackKing;
        b[SquareA1] = WhiteRook;
        b[SquareA8] = BlackKnight;
        return b;
    };

    const Position pos_init(init_board(), State(NoCastles, FileCNT, White, 7));
    Position pos = pos_init;
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
    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: En passant is reversible", "[position]") {
    auto init_board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE1] = WhiteKing;
        b[SquareE8] = BlackKing;
        b[SquareE5] = WhitePawn;
        b[SquareD5] = BlackPawn;
        return b;
    };

    const Position pos_init(init_board(), State(NoCastles, FileCNT, White, 12));
    Position pos = pos_init;
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
    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Short castle moves king and rook", "[position]") {
    auto init_board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE1] = WhiteKing;
        b[SquareH1] = WhiteRook;
        b[SquareE8] = BlackKing;
        return b;
    };

    const Position pos_init(init_board(), State(WhiteShortCastles, FileCNT, White, 3));
    Position pos = pos_init;
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
    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Promotion capture is reversible", "[position]") {
    auto init_board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE1] = WhiteKing;
        b[SquareE8] = BlackKing;
        b[SquareA7] = WhitePawn;
        b[SquareB8] = BlackRook;
        return b;
    };

    const Position pos_init(init_board(), State(NoCastles, FileCNT, White, 25));
    Position pos = pos_init;
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
    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Castling rights updated", "[position]") {
    auto init_board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE1] = WhiteKing;
        b[SquareA1] = b[SquareH1] = WhiteRook;
        return b;
    };

    const Position pos_init(init_board(), State(WhiteCastles, FileCNT, White, 0));
    Position pos = pos_init;
    pos.do_move({SquareA1, SquareA2, QuietMove});

    CHECK_FALSE(pos.state().can_castles(WhiteLongCastles));
    CHECK(pos.state().can_castles(WhiteShortCastles));

    pos.undo_move();
    check_equality(pos, pos_init);

    pos.do_move({SquareE1, SquareE2, QuietMove});
    CHECK_FALSE(pos.state().can_castles(WhiteLongCastles));
    CHECK_FALSE(pos.state().can_castles(WhiteShortCastles));

    pos.undo_move();
    check_equality(pos, pos_init);
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
    auto init_board = []() {
        std::array<Piece, SquareCNT> b;
        b.fill(PieceCNT);
        b[SquareE1] = WhiteKing;
        b[SquareH1] = WhiteRook;
        b[SquareE8] = BlackKing;
        b[SquareH4] = BlackQueen;
        return b;
    };

    const Position pos_init(init_board(), State(BothCastles, FileCNT, Black, 4));
    Position pos = pos_init;
    pos.do_move({SquareH4, SquareH1, Capture});

    const State state = pos.state();
    const std::array<Piece, SquareCNT> &board = pos.pieces().board();

    CHECK(board[SquareH1] == BlackQueen);
    CHECK(state.side_to_move() == White);
    CHECK_FALSE(state.can_castles(WhiteShortCastles));

    check_consistency(pos);
    pos.undo_move();
    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Illegal pseudo moves", "[position]") {
    SECTION("King move into pawns attack") {
        auto init_board = []() {
            std::array<Piece, SquareCNT> b;
            b.fill(PieceCNT);
            b[SquareE1] = WhiteKing;
            b[SquareD3] = BlackPawn;
            return b;
        };

        const Position pos_init(init_board(), State(NoCastles, FileCNT, White, 0));
        Position pos = pos_init;

        CHECK_FALSE(pos.try_do_pseudo({SquareE1, SquareE2, QuietMove}));

        check_consistency(pos);
        check_equality(pos, pos_init);
    }

    SECTION("Pinned piece move") {
        auto init_board = []() {
            std::array<Piece, SquareCNT> b;
            b.fill(PieceCNT);
            b[SquareE1] = WhiteKing;
            b[SquareE2] = WhiteBishop;
            b[SquareE3] = BlackRook;
            return b;
        };

        const Position pos_init(init_board(), State(NoCastles, FileCNT, White, 0));
        Position pos = pos_init;

        CHECK_FALSE(pos.try_do_pseudo({SquareE2, SquareD1, QuietMove}));

        check_consistency(pos);
        check_equality(pos, pos_init);
    }

    SECTION("Castling through attacked square") {
        auto init_board = []() {
            std::array<Piece, SquareCNT> b;
            b.fill(PieceCNT);
            b[SquareE1] = WhiteKing;
            b[SquareH1] = WhiteRook;
            b[SquareE2] = BlackPawn;
            return b;
        };

        const Position pos_init(init_board(), State(WhiteShortCastles, FileCNT, White, 0));
        Position pos = pos_init;

        CHECK_FALSE(pos.try_do_pseudo({SquareE1, SquareG1, ShortCastle}));

        check_consistency(pos);
        check_equality(pos, pos_init);
    }
}