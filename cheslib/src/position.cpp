#include "cheslib/types.hpp"

#include "position.hpp"
#include "utils.hpp"
#include "zobrist.hpp"

namespace cheslib {

namespace {

// left index: side
// right index: 0=long, 1=short
constexpr Square RookInitial[2][2] = {{SquareA1, SquareH1}, {SquareA8, SquareH8}};
constexpr Square RookCastled[2][2] = {{SquareD1, SquareF1}, {SquareD8, SquareF8}};

constexpr std::array<CastleFlag, SquareCNT> castling_mask = []() {
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

template <Side Us>
void Position::do_move_of(const Move move) {
    assert(_state.side_to_move() == Us);

    const Square from = move.from();
    const Square to = move.to();
    const MoveFlag move_flag = move.flag();
    const State old_state = _state;

    { // handle capture
        const ZKey old_key = _key;
        Piece captured = PieceCNT;

        if (move.is_capture()) {
            Square capture_sq = (move_flag == EnPassant) ? utils::square_behind<Us>(to) : to;
            captured = _pieces.remove<Side(!Us)>(capture_sq);
            _key ^= zobrist::piece(captured, capture_sq);
        }

        _history.push(old_key, move, old_state, captured);
    }

    { // move piece
        const Piece after = move.is_promotion() ? utils::piece_of<Us>(move.promo_piece()) : _pieces.at(from);
        const Piece before = _pieces.remove<Us>(from);
        _pieces.put<Us>(to, after);
        _key ^= zobrist::piece(after, to);
        _key ^= zobrist::piece(before, from);

        // rule 50
        if (move.is_capture() || before == utils::piece_of<Us>(Pawn)) {
            _state.reset_rule50();
        } else {
            _state.increment_rule50();
        }
    }

    { // move rook if castling
        bool is_short = move_flag == ShortCastle;
        if (is_short || move_flag == LongCastle) {
            constexpr Piece rook = utils::piece_of<Us>(Rook);
            Square rook_to = RookCastled[Us][is_short];
            Square rook_from = RookInitial[Us][is_short];
            _pieces.move<Us>(rook_from, rook_to);
            _key ^= zobrist::piece(rook, rook_from);
            _key ^= zobrist::piece(rook, rook_to);
        }
    }

    { // castling state
        CastleFlag old_castles = old_state.castle_flag();
        CastleFlag new_castles = CastleFlag(old_castles & ~(castling_mask[from] | castling_mask[to]));
        _state.set_castles(new_castles);
        _key ^= zobrist::castling(old_castles);
        _key ^= zobrist::castling(new_castles);
    }

    { // en passant state
        File old_ep = old_state.en_passant();
        File new_ep = (move_flag == DoublePawnPush) ? utils::file_of(from) : FileCNT;
        _state.set_en_passant(new_ep);
        _key ^= zobrist::en_passant(old_ep);
        _key ^= zobrist::en_passant(new_ep);
    }

    // switch side
    _state.switch_side();
    _key ^= zobrist::side();
}

template <Side Us>
void Position::undo_move_of() {
    assert(_history.size() > 0);
    const auto &[key, move, state, captured] = _history.pop();
    assert(state.side_to_move() == Us);

    const Square to = move.to();
    const Square from = move.from();
    const MoveFlag flag = move.flag();

    { // undo moved piece
        Piece moved = _pieces.remove<Us>(to);
        if (move.is_promotion()) {
            moved = utils::piece_of<Us>(Pawn); // was a pawn
        }
        _pieces.put<Us>(from, moved);
    }

    // undo captured piece
    if (move.is_capture()) {
        Square enemy = (flag == EnPassant) ? utils::square_behind<Us>(to) : to;
        _pieces.put<Side(!Us)>(enemy, captured);

        // undo castled rook
    } else if (flag == ShortCastle || flag == LongCastle) {
        bool is_short = flag == ShortCastle;
        Square rookTo = RookCastled[Us][is_short];
        Square rookFrom = RookInitial[Us][is_short];
        _pieces.move<Us>(rookTo, rookFrom);
    }

    _state = state;
    _key = key;
}

// explicit instantiate for inlined do_move/undo_move
template void Position::do_move_of<White>(Move);
template void Position::do_move_of<Black>(Move);
template void Position::undo_move_of<White>();
template void Position::undo_move_of<Black>();

} // namespace cheslib
