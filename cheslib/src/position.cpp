#include "position.hpp"
#include "attacks.hpp"
#include "zobrist.hpp"

namespace cheslib {

Position::Position(Pieces &&pieces, State state)
    : _pieces(std::move(pieces)),
      _state(state),
      _key(zobrist::hash(_pieces.board(), _state)),
      _history() {}

Position Position::initial() {
    return Position(Pieces::initial(), State::initial());
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

bool Position::is_in_check() const {
    Side us = _state.side_to_move();
    Square king = _pieces.king_of(us);
    return is_attacking(king, !us);
}

bool Position::is_drawn() const {
    return (_state.rule50_count() >= 100) || _history.is_threefold_repetition(_key);
}

bool Position::try_do_pseudo(Move move) {
    const Side us = _state.side_to_move();
    const Side enemy = !us;

    { // check castling path attacked
        MoveFlag move_flag = move.flag();
        if (move_flag == ShortCastle || move_flag == LongCastle) {
            Square from = move.from();
            Square to = move.to();
            Square between = Square((from + to) / 2);

            if (is_attacking(from, enemy) || is_attacking(between, enemy) || is_attacking(to, enemy)) {
                return false;
            }
        }
    }

    do_move(move);

    { // if king in check
        Square king = _pieces.king_of(us);
        if (is_attacking(king, enemy)) {
            undo_move();
            return false;
        }
    }

    return true;
}

bool Position::is_attacking(Square at, Side attacker) const {
    // us at Square can attack other Squares <=> us at other Squares can attack Square
    Bitboard all = _pieces.all();
    return (_pieces.get(attacker, Pawn) & attacks::pawn(at, !attacker)) ||
           (_pieces.get(attacker, Knight) & attacks::knight(at)) ||
           (_pieces.get(attacker, Bishop) & attacks::bishop(at, all)) ||
           (_pieces.get(attacker, Rook) & attacks::rook(at, all)) ||
           (_pieces.get(attacker, Queen) & attacks::queen(at, all)) ||
           (_pieces.get(attacker, King) & attacks::king(at));
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
    const auto [key, move, state, captured] = _history.pop();

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

void Position::do_move(const Move move) {
    const Side us = _state.side_to_move();
    const Square from = move.from();
    const Square to = move.to();
    const MoveFlag move_flag = move.flag();
    const State old_state = _state;

    { // handle capture
        ZobristKey old_key = _key;
        Piece captured = PieceCNT;

        if (move.is_capture()) {
            Square capture_sq = (move_flag == EnPassant) ? types::square_behind(us, to) : to;
            captured = _pieces.remove(capture_sq);
            _key ^= zobrist::piece(captured, capture_sq);
        }

        _history.push({old_key, move, old_state, captured});
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

    // castling rook
    if (move_flag == ShortCastle || move_flag == LongCastle) {
        bool is_short = move_flag == ShortCastle;
        Square rook_to = RookCastled[us][is_short];
        Square rook_from = RookInitial[us][is_short];
        Piece rook = types::piece_of(us, Rook);

        _pieces.move(rook_from, rook_to);
        _key ^= zobrist::piece(rook, rook_from) ^ zobrist::piece(rook, rook_to);
    }

    { // castling state
        CastleFlag revoke = CastleFlag(CastlingMasks[from] | CastlingMasks[to]);
        _state.revoke_castles(revoke);
        CastleFlag old_flag = old_state.castle_flag();
        CastleFlag new_flag = _state.castle_flag();
        _key ^= zobrist::castling(old_flag) ^ zobrist::castling(new_flag);
    }

    { // en passant state
        File new_ep = FileCNT;
        if (move_flag == DoublePawnPush) {
            Square ep_square = types::square_behind(us, to);
            Bitboard enemy_mask = attacks::pawn(ep_square, us); // us attack enemy <=> enemy attack us
            bool can_enemy_en_passant = enemy_mask & _pieces.get(!us, Pawn);
            if (can_enemy_en_passant) {
                new_ep = types::file_of(to);
            }
        }
        _state.set_en_passant(new_ep);
        _key ^= zobrist::en_passant(old_state.en_passant()) ^ zobrist::en_passant(new_ep);
    }

    // switch side
    _state.switch_side();
    _key ^= zobrist::side();
}

} // namespace cheslib
