#include "position.hpp"
#include "utils.hpp"
#include "zobrist.hpp"

namespace ches {

void Position::undo_move() {
    const auto &[key, move, state, captured] = _history.pop();

    _key = key;
    _state = state;

    const Square to = move.to();
    const MoveFlag flag = move.flag();

    if (state.side_to_move() == White) {
        undo_moved_piece<White>(move.from(), to, move.isPromotion());
        undo_captured_piece<White>(to, captured, flag == EnPassant);
        undo_castling<White>(flag);
    } else {
        undo_moved_piece<Black>(move.from(), to, move.isPromotion());
        undo_captured_piece<Black>(to, captured, flag == EnPassant);
        undo_castling<Black>(flag);
    }
}

template <Side Us>
void Position::undo_moved_piece(Square from, Square to, bool is_promotion) {
    Piece piece = _board[to];
    _board[to] = PieceCNT;
    _pieces.unset(to, piece);

    if (is_promotion) {
        piece = piece_of<Us>(Pawn); // it was a pawn
    }
    _board[from] = piece;
    _pieces.set(from, piece);
}

template <Side Us>
void Position::undo_captured_piece(Square to, Piece captured, bool is_en_passant) {
    if (captured >= PieceCNT) {
        return;
    }

    constexpr Direction forward = (Us == White) ? Up : Down;
    Square enemy = is_en_passant ? Square(to - (int)forward) : to;
    _board[enemy] = captured;
    _pieces.set(enemy, captured);
}

template <Side Us>
void Position::undo_castling(MoveFlag flag) {
    bool is_short = flag == ShortCastle;
    if (!is_short && flag != LongCastle) {
        return;
    }

    Square rookTo = _rook_castled[Us][is_short];
    Square rookFrom = _rook_initial[Us][is_short];
    Piece rook = _board[rookTo];

    _board[rookTo] = PieceCNT;
    _board[rookFrom] = rook;
    _pieces.unset(rookTo, rook);
    _pieces.set(rookFrom, rook);
}

// template <Side Us>
// StateInfo update_castle_rights(StateInfo state, Piece moved, Square from, Square to, Piece captured) {
//     constexpr Square short_rook = (Us == White) ? SquareH1 : SquareH8;
//     constexpr Square long_rook = (Us == White) ? SquareA1 : SquareA8;

//     if (moved == King) {
//         state.revoke_short_castle<Us>();
//         state.revoke_long_castle<Us>();

//     } else if (moved == Rook) {
//         if (from == short_rook) {
//             state.revoke_short_castle<Us>();
//         } else if (from == long_rook) {
//             state.revoke_long_castle<Us>();
//         }
//     }

//     constexpr Side them = Side(!Us);
//     if (captured == piece_of<them>(Rook)) {
//         if (to == short_rook) {
//             state.revoke_short_castle<them>();
//         } else if (to == long_rook) {
//             state.revoke_long_castle<them>();
//         }
//     }
// }

void Position::do_move(Move move) {
}
// void Position::do_move(Move move) {
//     const StateInfo old_state = _state;
//     const Side them = Side(!us);

//     const Square from = move.from();
//     const Square to = move.to();
//     const MoveFlag flag = move.flag();

//     if (_state.side_to_move() == White) {
//         update_castle_rights<White>(_state, type_of(_board[from]), from, to, type_of(_board[to]));

//     } else {
//         do_move<Black>(move);
//     }

//     // Capture info
//     Piece captured = PieceCNT;
//     Square capture_sq = to;
//     if (flag == EnPassant) {
//         const Direction forward = (us == White) ? Up : Down;
//         capture_sq = Square(to - (int)forward);
//         captured = piece_of(Pawn, them);
//     } else if (move.isCapture()) {
//         captured = _board[to];
//     }

//     const ZKey old_key = _key;

//     // --- Update board and bitboards ---
//     Piece moving = _board[from];
//     _board[from] = PieceCNT;
//     _pieces.unset(from, moving);
//     _key ^= zobrist_piece(moving, from);

//     if (captured != PieceCNT) {
//         _board[capture_sq] = PieceCNT;
//         _pieces.unset(capture_sq, captured);
//         _key ^= zobrist_piece(captured, capture_sq);
//     }

//     Piece dst = moving;
//     if (move.isPromotion()) {
//         dst = piece_of(move.promotion_piece(), us);
//     }
//     _board[to] = dst;
//     _pieces.set(to, dst);
//     _key ^= zobrist_piece(dst, to);

//     if (flag == ShortCastle || flag == LongCastle) {
//         const bool is_short = (flag == ShortCastle);
//         const Square rfrom = _rook_initial[us][is_short];
//         const Square rto = _rook_castled[us][is_short];
//         const Piece rook = _board[rfrom];
//         _board[rfrom] = PieceCNT;
//         _board[rto] = rook;
//         _pieces.unset(rfrom, rook);
//         _pieces.set(rto, rook);
//         _key ^= zobrist_piece(rook, rfrom);
//         _key ^= zobrist_piece(rook, rto);
//     }

//     // --- Update state (copy, then modify) ---
//     StateInfo new_state = old_state;

//     // Castling rights
//     // new_state = update_castle_rights<White>(new_state, moving, from, to, captured);

//     // Update Zobrist for castle changes
//     _key ^= zobrist_castling(old_state);
//     _key ^= zobrist_castling(new_state);

//     // En passant file
//     File new_ep = FileCNT;
//     if (type_of(moving) == Pawn && std::abs(static_cast<int>(to) - static_cast<int>(from)) == 16) {
//         new_ep = file_of(to);
//     }
//     _key ^= zobrist_en_passant(old_state.ep_file());
//     _key ^= zobrist_en_passant(new_ep);
//     new_state.set_en_passant(new_ep);

//     // Rule50
//     if (type_of(moving) == Pawn || captured != PieceCNT) {
//         new_state.reset_rule50();
//     } else {
//         new_state.increment_rule50();
//     }

//     // Side to move
//     new_state.switch_side();
//     _key ^= zobrist_side_to_move(); // toggle side in hash

//     // --- Push undo info (old key, move, old state, captured) ---
//     _history.push(old_key, move, old_state, captured);

//     // --- Apply new state ---
//     _state = new_state;
// }

} // namespace ches
