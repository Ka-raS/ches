#include <array>
#include <bit>

#include "attack_tables.hpp"
#include "cheslib/move.hpp"
#include "cheslib/types.hpp"
#include "position.hpp"
#include "utils.hpp"

namespace cheslib {

MoveList Position::generate_pseudo_legals() const {
    MoveList moves;

    if (_state.is_blacks_turn()) {
        generate_pawn_moves<true>(moves);
        generate_non_pawn_moves<true>(moves);
        generate_castling_moves<true>(moves);
    } else {
        generate_pawn_moves<false>(moves);
        generate_non_pawn_moves<false>(moves);
        generate_castling_moves<false>(moves);
    }

    return moves;
}

template <bool IsBlack>
void Position::generate_non_pawn_moves(MoveList &moves) const {
    const Bitboard not_us = ~(IsBlack ? _pieces.black() : _pieces.white());
    const Bitboard all = _pieces.all();

    for (Piece p = KNIGHT; p <= KING; ++p) {
        Bitboard bb = _pieces.get<IsBlack>(p);

        while (bb) {
            Square from = pop_lsb(bb);
            Bitboard attacks;

            switch (p) {
            case KNIGHT:
                attacks = not_us & knight_attacks(from);
                break;
            case BISHOP:
                attacks = not_us & bishop_attacks(from, all);
                break;
            case ROOK:
                attacks = not_us & rook_attacks(from, all);
                break;
            case QUEEN:
                attacks = not_us & queen_attacks(from, all);
                break;
            case KING:
                attacks = not_us & king_attacks(from);
                break;
            }

            while (attacks) {
                Square to = pop_lsb(attacks);
                MoveFlag flag = has_square(all, to) ? CAPTURE : QUIET_MOVE;
                moves.add(from, to, flag);
            }
        }
    }
}

template <bool IsBlack>
void Position::generate_castling_moves(MoveList &moves) const {
    const Bitboard all = _pieces.all();

    if constexpr (IsBlack) {
        constexpr Bitboard short_blockers = make_bitboard(SQUARE_F8, SQUARE_G8);
        constexpr Bitboard long_blockers = make_bitboard(SQUARE_D8, SQUARE_C8, SQUARE_B8);

        if (_state.can_black_short_castle() && !(all & short_blockers)) {
            moves.add(SQUARE_E8, SQUARE_G8, SHORT_CASTLE);
        }
        if (_state.can_black_long_castle() && !(all & long_blockers)) {
            moves.add(SQUARE_E8, SQUARE_C8, LONG_CASTLE);
        }

    } else {
        constexpr Bitboard short_blockers = make_bitboard(SQUARE_F1, SQUARE_G1);
        constexpr Bitboard long_blockers = make_bitboard(SQUARE_D1, SQUARE_C1, SQUARE_B1);

        if (_state.can_white_short_castle() && !(all & short_blockers)) {
            moves.add(SQUARE_E1, SQUARE_G1, SHORT_CASTLE);
        }
        if (_state.can_white_long_castle() && !(all & long_blockers)) {
            moves.add(SQUARE_E1, SQUARE_C1, LONG_CASTLE);
        }
    }
}


template <bool IsBlack>
void Position::generate_pawn_moves(MoveList &moves) const {
    constexpr auto move = [](Bitboard bb, Direction dir) constexpr -> Bitboard {
        return IsBlack ? (bb << dir) : (bb >> -dir);
    };

    constexpr Direction push_dir = IsBlack ? DOWN : UP;
    constexpr Bitboard promo_bb = IsBlack ? rank_bitboard(RANK_1) : rank_bitboard(RANK_8);
    const Bitboard empty = ~_pieces.all();
    const Bitboard pawns = _pieces.get<IsBlack>(PAWN);
    const Bitboard push_1 = empty & move(pawns, push_dir);

    { // single push
        Bitboard promo_push = push_1 & promo_bb;
        Bitboard normal_push = push_1 & ~promo_bb;

        while (promo_push) {
            Square to = pop_lsb(promo_push);
            Square from = Square(to - (int)push_dir);
            for (MoveFlag flag = QUEEN_PROMO; flag >= KNIGHT_PROMO; --flag) {
                moves.add(from, to, flag);
            }
        }
        while (normal_push) {
            Square to = pop_lsb(normal_push);
            Square from = Square(to - (int)push_dir);
            moves.add(from, to, QUIET_MOVE);
        }
    }

    { // double push
        constexpr Bitboard push_2_target = IsBlack ? rank_bitboard(RANK_5) : rank_bitboard(RANK_4);
        Bitboard push_2 = empty & push_2_target & move(pawns, push_dir);

        while (push_2) {
            Square to = pop_lsb(push_2);
            Square from = Square(to - 2 * push_dir);
            moves.add(from, to, DOUBLE_PAWN_PUSH);
        }
    }

    { // captures
        // black can't go left to A file, right to H file
        // white can't go left to H file, right to A file
        constexpr Bitboard left_target = IsBlack ? ~file_bitboard(FILE_A) : ~file_bitboard(FILE_H);
        constexpr Bitboard right_target = IsBlack ? ~file_bitboard(FILE_H) : ~file_bitboard(FILE_A);
        constexpr Direction left_dir = Direction(push_dir + LEFT);
        constexpr Direction right_dir = Direction(push_dir + RIGHT);

        const Bitboard them = IsBlack ? _pieces.white() : _pieces.black();
        const Bitboard left_cap = them & left_target & move(pawns, left_dir);
        const Bitboard right_cap = them & right_target & move(pawns, right_dir);

        Bitboard normal_left = left_cap & ~promo_bb;
        Bitboard normal_right = right_cap & ~promo_bb;
        Bitboard promo_left = left_cap & promo_bb;
        Bitboard promo_right = right_cap & promo_bb;

        while (promo_left) {
            Square to = pop_lsb(promo_left);
            Square from = Square(to - (int)left_dir);
            for (MoveFlag flag = QUEEN_PROMO_CAP; flag >= KNIGHT_PROMO_CAP; --flag) {
                moves.add(from, to, flag);
            }
        }
        while (promo_right) {
            Square to = pop_lsb(promo_right);
            Square from = Square(to - (int)right_dir);
            for (MoveFlag flag = QUEEN_PROMO_CAP; flag >= KNIGHT_PROMO_CAP; --flag) {
                moves.add(from, to, flag);
            }
        }
        while (normal_left) {
            Square to = pop_lsb(normal_left);
            Square from = Square(to - (int)left_dir);
            moves.add(from, to, CAPTURE);
        }
        while (normal_right) {
            Square to = pop_lsb(normal_right);
            Square from = Square(to - (int)right_dir);
            moves.add(from, to, CAPTURE);
        }
    }

    { // en croissant
        const File ep_file = _state.en_passant();
        if (ep_file >= FILE_CNT) {
            return;
        }
        const Square ep_square = make_square(ep_file, IsBlack ? RANK_3 : RANK_6);

        // white at ep_square attacks black at p <=> black at p attacks white at ep_square
        const Bitboard p = pawn_attacks<!IsBlack>(ep_square);
        Bitboard attackers = pawns & p;
        while (attackers) {
            Square from = pop_lsb(attackers);
            moves.add(from, ep_square, EN_PASSANT);
        }
    }
}

} // namespace cheslib