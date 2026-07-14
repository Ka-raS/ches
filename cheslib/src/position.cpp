#include "position.hpp"
#include "attacks.hpp"
#include "zobrist.hpp"

namespace cheslib {

Position::Position(const PositionState state, const std::array<Piece, SquareCNT> &board) :
    _pieces{board},
    _state{state},
    _key{zobrist::hash(board, _state)} {}

Position Position::initial() {
    return Position{PositionState::initial(), Pieces::initial()};
}

const Pieces &Position::pieces() const {
    return _pieces;
}

PositionState Position::state() const {
    return _state;
}

ZobristKey Position::key() const {
    return _key;
}

bool Position::is_in_check() const {
    const Side us = _state.side_to_move();
    const Square king = _pieces.king_of(us);
    return is_attacking(king, !us);
}

bool Position::is_50move_draw() const {
    return _state.rule50_count() >= 100;
}

bool Position::is_3fold_repetition() const {
    int count = 1; // current position

    for (const HistoryEntry &entry : _history) {
        if (entry.key == _key) {
            ++count;
        }
    }

    return count >= 3;
}

bool Position::is_insufficient_material() const {
    return !_pieces.get(WhitePawn) && !_pieces.get(BlackPawn) && //
           !_pieces.get(WhiteRook) && !_pieces.get(BlackRook) && //
           !_pieces.get(WhiteQueen) && !_pieces.get(BlackQueen) &&
           (_pieces.count(WhiteBishop) + _pieces.count(WhiteKnight) <= 1) &&
           (_pieces.count(BlackBishop) + _pieces.count(BlackKnight) <= 1);
}

void Position::trim_history() {
    // first move is irreversible
    assert(_history.size() > 0 && _history[0].state.rule50_count() == 0);

    const HistoryEntry top = _history.back();
    if (top.state.rule50_count() == 0) {
        _history.clear();
        _history.emplace_back(top);
    }
}

bool Position::is_attacking(const Square at, const Side attacker) const {
    const Bitboard all = _pieces.all();
    // us at Square can attack other Squares <=> us at other Squares can attack Square

    // clang-format off
    return (_pieces.get(piece_of(attacker, Pawn))   & attacks::pawn(at, !attacker)) ||
           (_pieces.get(piece_of(attacker, Knight)) & attacks::knight(at))          ||
           (_pieces.get(piece_of(attacker, Bishop)) & attacks::bishop(at, all))     ||
           (_pieces.get(piece_of(attacker, Rook))   & attacks::rook(at, all))       ||
           (_pieces.get(piece_of(attacker, Queen))  & attacks::queen(at, all))      ||
           (_pieces.get(piece_of(attacker, King))   & attacks::king(at));
    // clang-format on
}

bool Position::try_do_pseudo(const Move move) {
    const Side us = _state.side_to_move();
    const Side enemy = !us;

    { // check castling path attacked
        const MoveFlag move_flag = move.flag();
        if (move_flag == ShortCastle || move_flag == LongCastle) {
            const Square from = move.from();
            const Square to = move.to();
            const Square between = Square((from + to) / 2);

            if (is_attacking(from, enemy) || is_attacking(between, enemy) || is_attacking(to, enemy)) {
                return false;
            }
        }
    }

    do_legal(move);

    { // if king in check
        const Square king = _pieces.king_of(us);
        if (is_attacking(king, enemy)) {
            undo_move();
            return false;
        }
    }

    return true;
}

namespace {

// left index: side
// right index: 0=long, 1=short
constexpr Square RookInitial[2][2] = {{SquareA1, SquareH1}, {SquareA8, SquareH8}};
constexpr Square RookCastled[2][2] = {{SquareD1, SquareF1}, {SquareD8, SquareF8}};

constexpr std::array<CastleFlag, SquareCNT> CastlingMasks = [] {
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
    const auto [key, state, move, captured] = _history.back();
    _history.pop_back();

    const Side us = state.side_to_move();
    const Square to = move.to();
    const Square from = move.from();
    const MoveFlag flag = move.flag();

    { // undo moved piece
        Piece moved = _pieces.remove(to);
        if (move.is_promotion()) {
            moved = piece_of(us, Pawn); // was a pawn
        }
        _pieces.put(moved, from);
    }

    if (move.is_capture()) { // undo captured piece
        const Square enemy = (flag == EnPassant) ? square_behind(us, to) : to;
        _pieces.put(captured, enemy);

    } else if (flag == ShortCastle || flag == LongCastle) { // undo castled rook
        const bool is_short = flag == ShortCastle;
        const Square rookTo = RookCastled[us][is_short];
        const Square rookFrom = RookInitial[us][is_short];
        _pieces.move(rookTo, rookFrom);
    }

    _state = state;
    _key = key;
}

void Position::do_legal(const Move move) {
    const Side us = _state.side_to_move();
    const Square from = move.from();
    const Square to = move.to();
    const MoveFlag move_flag = move.flag();
    const PositionState old_state = _state;

    { // handle capture
        const ZobristKey old_key = _key;
        Piece captured = PieceCNT;

        if (move.is_capture()) {
            const Square capture_sq = (move_flag == EnPassant) ? square_behind(us, to) : to;
            captured = _pieces.remove(capture_sq);
            _key ^= zobrist::piece(captured, capture_sq);
        }

        _history.emplace_back(old_key, old_state, move, captured);
    }

    { // move piece
        const Piece after = move.is_promotion() ? piece_of(us, move.promoted_piece()) : _pieces.at(from);
        const Piece before = _pieces.remove(from);

        _pieces.put(after, to);
        _key ^= zobrist::piece(before, from) ^ zobrist::piece(after, to);

        // rule 50
        if (move.is_capture() || type_of(before) == Pawn) {
            _state.reset_rule50();
        } else {
            _state.increment_rule50();
        }
    }

    // castling rook
    if (move_flag == ShortCastle || move_flag == LongCastle) {
        const bool is_short = move_flag == ShortCastle;
        const Square rook_to = RookCastled[us][is_short];
        const Square rook_from = RookInitial[us][is_short];
        const Piece rook = piece_of(us, Rook);

        _pieces.move(rook_from, rook_to);
        _key ^= zobrist::piece(rook, rook_from) ^ zobrist::piece(rook, rook_to);
    }

    { // castling state
        const CastleFlag revoke = CastleFlag(CastlingMasks[from] | CastlingMasks[to]);
        _state.revoke_castles(revoke);

        const CastleFlag old_flag = old_state.castle_flag();
        const CastleFlag new_flag = _state.castle_flag();
        _key ^= zobrist::castling(old_flag) ^ zobrist::castling(new_flag);
    }

    { // en passant state
        File new_ep = FileCNT;
        if (move_flag == DoublePawnPush) {
            const Square ep_square = square_behind(us, to);
            const Bitboard enemy_mask = attacks::pawn(ep_square, us); // us attack enemy <=> enemy attack us
            const bool can_enemy_en_passant = enemy_mask & _pieces.get(piece_of(!us, Pawn));
            if (can_enemy_en_passant) {
                new_ep = file_of(to);
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
