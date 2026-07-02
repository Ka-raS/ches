#include "position.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace ::cheslib;

namespace {

void check_equality(const Position &pos1, const Position &pos2) {
    CHECK(pos1.state() == pos2.state());
    CHECK(pos1.pieces().board() == pos2.pieces().board());
    CHECK(pos1.key() == pos2.key());
}

void check_consistency(const Position &pos) {
    const Pieces &pieces = pos.pieces();
    const std::array<Piece, SquareCNT> &board = pieces.board();

    auto count = [&board](const Piece target) -> int {
        int cnt = 0;
        for (const Piece piece : board) {
            if (piece == target) {
                ++cnt;
            }
        }
        return cnt;
    };

    Bitboard all = 0;
    Bitboard white = 0;
    Bitboard black = 0;

    for (Piece piece = Piece(0); piece < PieceCNT; ++piece) {
        const Bitboard bb = pieces.get(piece);
        CHECK(count(piece) == std::popcount(bb));

        all |= bb;
        if (side_of(piece) == White) {
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

        CHECK(has_square(all, sq) == has_piece);
        if (has_piece) {
            const Bitboard bb = pieces.get(piece);
            CHECK(has_square(bb, sq));
        }
    }
}

} // namespace

TEST_CASE("Position: Quiet move keeps full consistency", "[position]") {
    const Position pos_init = Position::initial();

    Position pos = pos_init;
    pos.do_legal(Move{SquareG1, SquareF3, QuietMove});

    const PositionState state = pos.state();
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
    const Position pos_init{PositionState{NoCastles, FileCNT, White, 0}, [] {
        std::array<Piece, SquareCNT> board;
        board.fill(PieceCNT);
        board[SquareE2] = WhitePawn;
        board[SquareD4] = BlackPawn;
        return board;
    }()};

    Position pos = pos_init;
    pos.do_legal(Move{SquareE2, SquareE4, DoublePawnPush});

    const PositionState state = pos.state();
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
    const Position pos_init{PositionState{NoCastles, FileCNT, White, 7}, [] {
        std::array<Piece, SquareCNT> board;
        board.fill(PieceCNT);
        board[SquareE1] = WhiteKing;
        board[SquareE8] = BlackKing;
        board[SquareA1] = WhiteRook;
        board[SquareA8] = BlackKnight;
        return board;
    }()};

    Position pos = pos_init;
    pos.do_legal(Move{SquareA1, SquareA8, Capture});

    const PositionState state = pos.state();
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
    const Position pos_init{PositionState{NoCastles, FileCNT, White, 12}, [] {
        std::array<Piece, SquareCNT> board;
        board.fill(PieceCNT);
        board[SquareE1] = WhiteKing;
        board[SquareE8] = BlackKing;
        board[SquareE5] = WhitePawn;
        board[SquareD5] = BlackPawn;
        return board;
    }()};

    Position pos = pos_init;
    pos.do_legal(Move{SquareE5, SquareD6, EnPassant});

    const PositionState state = pos.state();
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
    const Position pos_init{PositionState{WhiteShortCastles, FileCNT, White, 3}, [] {
        std::array<Piece, SquareCNT> board;
        board.fill(PieceCNT);
        board[SquareE1] = WhiteKing;
        board[SquareH1] = WhiteRook;
        board[SquareE8] = BlackKing;
        return board;
    }()};

    Position pos = pos_init;
    pos.do_legal(Move{SquareE1, SquareG1, ShortCastle});

    const PositionState state = pos.state();
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
    const Position pos_init{PositionState{NoCastles, FileCNT, White, 25}, [] {
        std::array<Piece, SquareCNT> board;
        board.fill(PieceCNT);
        board[SquareE1] = WhiteKing;
        board[SquareE8] = BlackKing;
        board[SquareA7] = WhitePawn;
        board[SquareB8] = BlackRook;
        return board;
    }()};

    Position pos = pos_init;
    pos.do_legal(Move{SquareA7, SquareB8, QueenPromoCap});

    const PositionState state = pos.state();
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
    const Position pos_init{PositionState{WhiteCastles, FileCNT, White, 0}, [] {
        std::array<Piece, SquareCNT> board;
        board.fill(PieceCNT);
        board[SquareE1] = WhiteKing;
        board[SquareA1] = board[SquareH1] = WhiteRook;
        return board;
    }()};

    Position pos = pos_init;
    pos.do_legal(Move{SquareA1, SquareA2, QuietMove});

    CHECK_FALSE(pos.state().can_castles(WhiteLongCastles));
    CHECK(pos.state().can_castles(WhiteShortCastles));

    pos.undo_move();
    check_equality(pos, pos_init);

    pos.do_legal(Move{SquareE1, SquareE2, QuietMove});
    CHECK_FALSE(pos.state().can_castles(WhiteLongCastles));
    CHECK_FALSE(pos.state().can_castles(WhiteShortCastles));

    pos.undo_move();
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Multiple dos then undos", "[position]") {
    const Position pos_init = Position::initial();

    Position pos = pos_init;
    pos.do_legal(Move{SquareE2, SquareE4, DoublePawnPush});
    pos.do_legal(Move{SquareA7, SquareA6, QuietMove});
    pos.do_legal(Move{SquareG1, SquareF3, QuietMove});
    pos.do_legal(Move{SquareB8, SquareC6, QuietMove});

    check_consistency(pos);

    pos.undo_move();
    pos.undo_move();
    pos.undo_move();
    pos.undo_move();

    check_consistency(pos);
    check_equality(pos, pos_init);
}

TEST_CASE("Position: Capturing rook revokes castling right", "[position]") {
    const Position pos_init{PositionState{BothCastles, FileCNT, Black, 4}, [] {
        std::array<Piece, SquareCNT> board;
        board.fill(PieceCNT);
        board[SquareE1] = WhiteKing;
        board[SquareH1] = WhiteRook;
        board[SquareE8] = BlackKing;
        board[SquareH4] = BlackQueen;
        return board;
    }()};

    Position pos = pos_init;
    pos.do_legal(Move{SquareH4, SquareH1, Capture});

    const PositionState state = pos.state();
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
        const Position pos_init{PositionState{NoCastles, FileCNT, White, 0}, [] {
            std::array<Piece, SquareCNT> board;
            board.fill(PieceCNT);
            board[SquareE1] = WhiteKing;
            board[SquareD3] = BlackPawn;
            return board;
        }()};

        Position pos = pos_init;
        CHECK_FALSE(pos.try_do_pseudo(Move{SquareE1, SquareE2, QuietMove}));

        check_consistency(pos);
        check_equality(pos, pos_init);
    }

    SECTION("Pinned piece move") {
        const Position pos_init{PositionState{NoCastles, FileCNT, White, 0}, [] {
            std::array<Piece, SquareCNT> board;
            board.fill(PieceCNT);
            board[SquareE1] = WhiteKing;
            board[SquareE2] = WhiteBishop;
            board[SquareE3] = BlackRook;
            return board;
        }()};

        Position pos = pos_init;
        CHECK_FALSE(pos.try_do_pseudo(Move{SquareE2, SquareD1, QuietMove}));

        check_consistency(pos);
        check_equality(pos, pos_init);
    }

    SECTION("Castling through attacked square") {
        const Position pos_init{PositionState{WhiteShortCastles, FileCNT, White, 0}, [] {
            std::array<Piece, SquareCNT> board;
            board.fill(PieceCNT);
            board[SquareE1] = WhiteKing;
            board[SquareH1] = WhiteRook;
            board[SquareE2] = BlackPawn;
            return board;
        }()};

        Position pos = pos_init;
        CHECK_FALSE(pos.try_do_pseudo(Move{SquareE1, SquareG1, ShortCastle}));

        check_consistency(pos);
        check_equality(pos, pos_init);
    }
}