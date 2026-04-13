#include "cheslib/types.hpp"

#include "position.hpp"
#include "utils.hpp"
#include "zobrist.hpp"

namespace cheslib {

namespace {

consteval std::array<CastleFlag, SquareCNT> init_castling_mask() {
    std::array<CastleFlag, SquareCNT> mask = {NoCastles};

    mask[SquareA1] = WhiteLongCastles;
    mask[SquareH1] = WhiteShortCastles;
    mask[SquareE1] = CastleFlag(WhiteShortCastles | WhiteLongCastles);

    mask[SquareA8] = BlackLongCastles;
    mask[SquareH8] = BlackShortCastles;
    mask[SquareE8] = CastleFlag(BlackShortCastles | BlackLongCastles);

    return mask;
}

constexpr std::array<CastleFlag, SquareCNT> castling_mask = init_castling_mask();

// left index: side
// right index: 0=long, 1=short
constexpr Square RookInitial[2][2] = {{SquareA1, SquareH1}, {SquareA8, SquareH8}};
constexpr Square RookCastled[2][2] = {{SquareD1, SquareF1}, {SquareD8, SquareF8}};

} // namespace

std::array<Piece, SquareCNT> Position::init_board(const PieceBitboards &pieces) {
    std::array<Piece, SquareCNT> board{};
    board.fill(PieceCNT);

    for (Piece piece = WhitePawn; piece < PieceCNT; ++piece) {
        Bitboard bb = pieces.get(piece);
        while (bb) {
            Square sq = pop_lsb(bb);
            board[sq] = piece;
        }
    }

    return board;
}

void Position::do_move(const Move move) {
    const Side us = _state.side_to_move();
    const Square from = move.from();
    const Square to = move.to();
    const MoveFlag move_flag = move.flag();
    const State old_state = _state;

    { // handle capture
        const ZKey old_key = _key;
        Piece captured = _board[to];

        if (move_flag == EnPassant) {
            Square capture_sq = square_behind(to, us);
            captured = _board[capture_sq];
            unset_piece(capture_sq);
        } else if (move.is_capture()) {
            unset_piece(to);
        }

        _history.push(old_key, move, old_state, captured);
    }

    { // move piece
        const Piece moved = move.is_promotion() ? piece_of(move.promotion_piece(), us) : _board[from];
        set_piece(to, moved);
        unset_piece(from);

        // rule 50
        if (move.is_promotion() || move.is_capture() || type_of(moved) == Pawn) {
            _state.reset_rule50();
        } else {
            _state.increment_rule50();
        }
    }

    { // move rook if castling
        const bool is_short = move_flag == ShortCastle;
        if (is_short || move_flag == LongCastle) {
            Square rook_from = RookInitial[us][is_short];
            Square rook_to = RookCastled[us][is_short];

            set_piece(rook_to, _board[rook_from]);
            unset_piece(rook_from);
        }
    }

    { // castling state
        const CastleFlag old_castles = old_state.castle_flag();
        const CastleFlag new_castles = CastleFlag(old_castles & ~(castling_mask[from] | castling_mask[to]));
        _key ^= zobrist_castling(old_castles);
        _key ^= zobrist_castling(new_castles);
        _state.set_castles(new_castles);
    }

    { // en passant state
        const File old_ep = old_state.en_passant();
        const File new_ep = (move_flag == DoublePawnPush) ? file_of(from) : FileCNT;
        _state.set_en_passant(new_ep);
        _key ^= zobrist_en_passant(old_ep);
        _key ^= zobrist_en_passant(new_ep);
    }

    // switch side
    _state.switch_side();
    _key ^= zobrist_side();
}

void Position::undo_move() {
    assert(!_history.empty());

    const auto &[key, move, state, captured] = _history.pop();
    const Side us = state.side_to_move();
    const Square to = move.to();
    const Square from = move.from();
    const MoveFlag flag = move.flag();

    { // undo moved piece
        Piece moved = move.is_promotion() ? piece_of(Pawn, us) : _board[to];
        set_piece_no_hash(from, moved);
        unset_piece_no_hash(to);
    }

    if (move.is_capture()) { // undo captured piece
        Square enemy = (flag == EnPassant) ? square_behind(to, us) : to;
        set_piece_no_hash(enemy, captured);

    } else if (flag == ShortCastle || flag == LongCastle) { // undo castled rook
        bool is_short = flag == ShortCastle;

        Square rookTo = RookCastled[us][is_short];
        Square rookFrom = RookInitial[us][is_short];
        Piece rook = _board[rookTo];

        set_piece_no_hash(rookFrom, rook);
        unset_piece_no_hash(rookTo);
    }

    _state = state;
    _key = key;
}

} // namespace cheslib
