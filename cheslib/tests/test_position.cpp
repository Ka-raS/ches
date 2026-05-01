#include <catch2/catch_test_macros.hpp>

#include "position.hpp"

using namespace cheslib;

namespace {

const Pieces NoPieces = []() {
    std::array<Piece, SquareCNT> board{};
    board.fill(PieceCNT);
    return board;
}();

int count(const Pieces &pieces, Piece piece) {
    Bitboard bb = types::side_of(piece) == White ? pieces.get<White>(types::type_of(piece))
                                                 : pieces.get<Black>(types::type_of(piece));
    return std::popcount(bb);
}

void check_consistency(const Position &pos) {
    const Pieces &pieces = pos.pieces();
    const std::array<Piece, SquareCNT> &board = pieces.board();

    Bitboard all = 0;
    Bitboard white = 0;
    Bitboard black = 0;

    auto count_board = [](const std::array<Piece, SquareCNT> &board, Piece piece) {
        int count = 0;
        for (Piece p : board) {
            if (p == piece) {
                ++count;
            }
        }
        return count;
    };

    for (PieceType type = Pawn; type <= King; ++type) {
        Bitboard white_bb = pieces.get<White>(type);
        Bitboard black_bb = pieces.get<Black>(type);

        CHECK(count_board(board, types::piece_of<White>(type)) == std::popcount(white_bb));
        CHECK(count_board(board, types::piece_of<Black>(type)) == std::popcount(black_bb));

        all |= white_bb | black_bb;
        white |= white_bb;
        black |= black_bb;
    }

    CHECK(all == pieces.all());
    CHECK(white == pieces.all<White>());
    CHECK(black == pieces.all<Black>());
    CHECK((white & black) == 0);

    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        const Piece piece = board[sq];
        const bool has_piece = piece < PieceCNT;

        CHECK(types::has_square(all, sq) == has_piece);
        if (has_piece) {
            Bitboard bb = types::side_of(piece) == White ? pieces.get<White>(types::type_of(piece))
                                                         : pieces.get<Black>(types::type_of(piece));
            CHECK(types::has_square(bb, sq));
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
        Pieces pieces = NoPieces;
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
        CHECK(count(pieces, BlackKnight) == 0);
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
        Pieces pieces = NoPieces;
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
        Pieces pieces = NoPieces;
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
        Pieces pieces = NoPieces;
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
        CHECK(count(pieces, BlackRook) == 0);
        CHECK(count(pieces, WhitePawn) == 0);
        CHECK(count(pieces, WhiteQueen) == 1);
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
        Pieces pieces = NoPieces;
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
        Pieces pieces = NoPieces;
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
