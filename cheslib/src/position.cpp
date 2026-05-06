#include "position.hpp"
#include "attacks.hpp"

namespace cheslib {

Position::Position(Pieces &&pieces, State state)
    : _pieces(std::move(pieces)), _state(state), _key(zobrist::hash(_pieces.board(), _state)), _history{},
      _history_size(0) {
}

const Pieces &Position::pieces() const {
    return _pieces;
}

State Position::state() const {
    return _state;
}

ZobristKey Position::key() const {
    return _key;
}

void Position::push_history(MoveEntry entry) {
    _history[_history_size] = entry;
    ++_history_size;
}

Position::MoveEntry Position::pop_history() {
    --_history_size;
    return std::move(_history[_history_size]);
}

Position Position::initial() {
    std::array<Piece, SquareCNT> board;
    board.fill(PieceCNT);

    board[SquareE1] = WhiteKing;
    board[SquareE8] = BlackKing;
    board[SquareD1] = WhiteQueen;
    board[SquareD8] = BlackQueen;

    board[SquareA1] = board[SquareH1] = WhiteRook;
    board[SquareA8] = board[SquareH8] = BlackRook;
    board[SquareC1] = board[SquareF1] = WhiteBishop;
    board[SquareC8] = board[SquareF8] = BlackBishop;
    board[SquareB1] = board[SquareG1] = WhiteKnight;
    board[SquareB8] = board[SquareG8] = BlackKnight;

    for (Square sq = SquareA2; sq <= SquareH2; ++sq) {
        board[sq] = WhitePawn;
    }
    for (Square sq = SquareA7; sq <= SquareH7; ++sq) {
        board[sq] = BlackPawn;
    }

    return Position(Pieces(std::move(board)), State(BothCastles, FileCNT, White, 0));
}

bool Position::try_do_pseudo(Move move) {
    const Side us = _state.side_to_move();
    const Side enemy = !us;
    const Square from = move.from();
    const Square to = move.to();
    const MoveFlag move_flag = move.flag();

    // check castling path attacked
    if (move_flag == ShortCastle || move_flag == LongCastle) {
        Square between = Square((from + to) / 2);
        if (is_attacked(from, enemy) || is_attacked(between, enemy) || is_attacked(to, enemy)) {
            return false;
        }
    }

    do_pseudo(move, us, from, to, move_flag);

    { // check king is attacked
        Piece king = types::piece_of(us, King);
        Bitboard king_bb = _pieces.get(king);
        Square king_sq = (Square)std::countr_zero(king_bb);
        if (is_attacked(king_sq, enemy)) {
            undo_move();
            return false;
        }
    }

    return true;
}

bool Position::is_attacked(Square at, Side us) const {
    Bitboard all = _pieces.all();
    // us at Square can attack other Squares <=> us at other Squares can attack Square

    // clang-format off
    Bitboard pawns   = _pieces.get(types::piece_of(us, Pawn))   & attacks::pawn(at, us);
    Bitboard knights = _pieces.get(types::piece_of(us, Knight)) & attacks::knight(at);
    Bitboard bishops = _pieces.get(types::piece_of(us, Bishop)) & attacks::bishop(at, all);
    Bitboard rooks   = _pieces.get(types::piece_of(us, Rook))   & attacks::rook(at, all);
    Bitboard queens  = _pieces.get(types::piece_of(us, Queen))  & attacks::queen(at, all);
    Bitboard king    = _pieces.get(types::piece_of(us, King))   & attacks::king(at);
    // clang-format on

    return pawns | knights | bishops | rooks | queens | king;
}

namespace {

// left index: side
// right index: 0=long, 1=short
constexpr Square RookInitial[2][2] = {{SquareA1, SquareH1}, {SquareA8, SquareH8}};
constexpr Square RookCastled[2][2] = {{SquareD1, SquareF1}, {SquareD8, SquareF8}};

constexpr std::array<CastleFlag, SquareCNT> CastlingMasks = []() {
    std::array<CastleFlag, SquareCNT> masks = {NoCastles};

    masks[SquareA1] = WhiteLongCastles;
    masks[SquareH1] = WhiteShortCastles;
    masks[SquareE1] = WhiteCastles;

    masks[SquareA8] = BlackLongCastles;
    masks[SquareH8] = BlackShortCastles;
    masks[SquareE8] = BlackCastles;

    return masks;
}();

} // namespace

void Position::undo_move() {
    const auto [key, move, state, captured] = pop_history();

    const Side us = state.side_to_move();
    const Square to = move.to();
    const Square from = move.from();
    const MoveFlag flag = move.flag();

    { // undo moved piece
        Piece moved = _pieces.remove(to);
        if (move.is_promotion()) {
            moved = types::piece_of(us, Pawn); // was a pawn
        }
        _pieces.put(from, moved);
    }

    if (move.is_capture()) { // undo captured piece
        Square enemy = (flag == EnPassant) ? types::square_behind(us, to) : to;
        _pieces.put(enemy, captured);

    } else if (flag == ShortCastle || flag == LongCastle) { // undo castled rook
        bool is_short = flag == ShortCastle;
        Square rookTo = RookCastled[us][is_short];
        Square rookFrom = RookInitial[us][is_short];
        _pieces.move(rookTo, rookFrom);
    }

    _state = state;
    _key = key;
}

void Position::do_pseudo(const Move move, const Side us, const Square from, const Square to, const MoveFlag move_flag) {
    const State old_state = _state;

    { // handle capture
        ZobristKey old_key = _key;
        Piece captured = PieceCNT;

        if (move.is_capture()) {
            Square capture_sq = (move_flag == EnPassant) ? types::square_behind(us, to) : to;
            captured = _pieces.remove(capture_sq);
            _key ^= zobrist::piece(captured, capture_sq);
        }

        push_history({old_key, move, old_state, captured});
    }

    { // move piece
        Piece after = move.is_promotion() ? types::piece_of(us, move.promoted_piece()) : _pieces.at(from);
        Piece before = _pieces.remove(from);

        _pieces.put(to, after);
        _key ^= zobrist::piece(before, from) ^ zobrist::piece(after, to);

        // rule 50
        if (move.is_capture() || before == types::piece_of(us, Pawn)) {
            _state.reset_rule50();
        } else {
            _state.increment_rule50();
        }
    }

    { // move rook if castling
        bool is_short = move_flag == ShortCastle;
        if (is_short || move_flag == LongCastle) {
            Square rook_to = RookCastled[us][is_short];
            Square rook_from = RookInitial[us][is_short];
            Piece rook = types::piece_of(us, Rook);

            _pieces.move(rook_from, rook_to);
            _key ^= zobrist::piece(rook, rook_from) ^ zobrist::piece(rook, rook_to);
        }
    }

    { // castling state
        CastleFlag old_flag = old_state.castle_flag();
        _state.revoke_castles(CastleFlag(CastlingMasks[from] | CastlingMasks[to]));
        _key ^= zobrist::castling(old_flag) ^ zobrist::castling(_state.castle_flag());
    }

    { // en passant state
        File old_ep = old_state.en_passant();
        File new_ep = (move_flag == DoublePawnPush) ? types::file_of(from) : FileCNT;
        _state.set_en_passant(new_ep);
        _key ^= zobrist::en_passant(old_ep) ^ zobrist::en_passant(new_ep);
    }

    // switch side
    _state.switch_side();
    _key ^= zobrist::side();
}

} // namespace cheslib
