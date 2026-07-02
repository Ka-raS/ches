#include "movegen.hpp"
#include "attacks.hpp"

namespace cheslib::movegen {

namespace {

constexpr MoveFlag operator--(MoveFlag &flag) {
    return flag = MoveFlag(flag - 1u);
}

template <Side Us>
void generate_non_pawn_moves(Array<MoveScore, 256> &moves, const Pieces &pieces) {
    const Bitboard enemy = pieces.all_of(!Us);
    const Bitboard not_us = ~pieces.all_of(Us);
    const Bitboard occupancy = pieces.all();

    for (PieceType type = Knight; type <= King; ++type) {
        Bitboard bb = pieces.get(Us, type);

        while (bb) {
            const Square from = pop_lsb(bb);
            Bitboard attacks = not_us;

            switch (type) {
            case Knight:
                attacks &= attacks::knight(from);
                break;

            case Bishop:
                attacks &= attacks::bishop(from, occupancy);
                break;
            case Rook:
                attacks &= attacks::rook(from, occupancy);
                break;

            case Queen:
                attacks &= attacks::queen(from, occupancy);
                break;

            case King:
                attacks &= attacks::king(from);
                break;

            default:
                assert(false);
            }

            while (attacks) {
                const Square to = pop_lsb(attacks);
                const MoveFlag flag = has_square(enemy, to) ? Capture : QuietMove;
                moves.emplace_back(Move{from, to, flag});
            }
        }
    }
}

template <Side Us>
void generate_castling_moves(Array<MoveScore, 256> &moves, const Pieces &pieces, const PositionState state) {
    const Bitboard occupancy = pieces.all();

    if constexpr (Us == White) {
        constexpr Bitboard short_blockers = bitboard_of(SquareF1, SquareG1);
        constexpr Bitboard long_blockers = bitboard_of(SquareD1, SquareC1, SquareB1);

        if (state.can_castles(WhiteShortCastles) && !(occupancy & short_blockers)) {
            moves.emplace_back(Move{SquareE1, SquareG1, ShortCastle});
        }
        if (state.can_castles(WhiteLongCastles) && !(occupancy & long_blockers)) {
            moves.emplace_back(Move{SquareE1, SquareC1, LongCastle});
        }

    } else {
        constexpr Bitboard short_blockers = bitboard_of(SquareF8, SquareG8);
        constexpr Bitboard long_blockers = bitboard_of(SquareD8, SquareC8, SquareB8);

        if (state.can_castles(BlackShortCastles) && !(occupancy & short_blockers)) {
            moves.emplace_back(Move{SquareE8, SquareG8, ShortCastle});
        }
        if (state.can_castles(BlackLongCastles) && !(occupancy & long_blockers)) {
            moves.emplace_back(Move{SquareE8, SquareC8, LongCastle});
        }
    }
}

// white pawn direction is > 0, black direction is < 0
template <Direction Dir>
Bitboard move_pawn(const Bitboard bb) {
    if constexpr (Dir > 0) {
        return bb << Dir;
    } else {
        return bb >> -Dir;
    }
};

template <Side Us>
void generate_single_pawn_pushes(Array<MoveScore, 256> &moves, const Bitboard pushed_1) {
    constexpr Bitboard promo_bb = (Us == White) ? bitboard_of(Rank8) : bitboard_of(Rank1);

    Bitboard promo_push = pushed_1 & promo_bb;
    Bitboard normal_push = pushed_1 & ~promo_bb;

    while (promo_push) {
        const Square to = pop_lsb(promo_push);
        const Square from = square_behind(Us, to);
        for (MoveFlag flag = QueenPromo; flag >= KnightPromo; --flag) {
            moves.emplace_back(Move{from, to, flag});
        }
    }
    while (normal_push) {
        const Square to = pop_lsb(normal_push);
        const Square from = square_behind(Us, to);
        moves.emplace_back(Move{from, to, QuietMove});
    }
}

template <Side Us>
void generate_double_pawn_pushes(Array<MoveScore, 256> &moves, const Bitboard pushed_1, const Bitboard empty) {
    constexpr Direction forward = (Us == White) ? North : South;
    constexpr Bitboard destination = (Us == White) ? bitboard_of(Rank4) : bitboard_of(Rank5);

    Bitboard pushed_2 = empty & destination & move_pawn<forward>(pushed_1);
    while (pushed_2) {
        const Square to = pop_lsb(pushed_2);
        const Square from = Square(to - 2 * forward);
        moves.emplace_back(Move{from, to, DoublePawnPush});
    }
}

template <Side Us>
void generate_en_croissants(Array<MoveScore, 256> &moves, const Bitboard our_pawns, const File ep_file) {
    assert(ep_file < FileCNT);

    const Square ep_square = square_of(ep_file, (Us == White) ? Rank6 : Rank3);
    const Bitboard us_attacked = attacks::pawn(ep_square, !Us); // enemy attack us <=> us attack enemy
    Bitboard our_attackers = our_pawns & us_attacked;

    assert(our_attackers != 0);
    while (our_attackers) {
        const Square from = pop_lsb(our_attackers);
        moves.emplace_back(Move{from, ep_square, EnPassant});
    }
}

template <Side Us, Direction Dir>
void generate_pawn_captures(Array<MoveScore, 256> &moves, const Bitboard our_pawns, const Bitboard enemy) {
    assert(Dir == East || Dir == West);

    constexpr bool is_white = Us == White;
    constexpr Direction capture_dir = Direction(Dir + (is_white ? North : South));
    constexpr Bitboard promo_bb = is_white ? bitboard_of(Rank8) : bitboard_of(Rank1);
    constexpr Bitboard mask = (Dir == East) ? ~bitboard_of(FileA) : ~bitboard_of(FileH);

    const Bitboard captures = enemy & mask & move_pawn<capture_dir>(our_pawns);
    Bitboard normal_captures = captures & ~promo_bb;
    Bitboard promo_captures = captures & promo_bb;

    while (promo_captures) {
        const Square to = pop_lsb(promo_captures);
        const Square from = Square(to - (int)capture_dir);
        for (MoveFlag flag = QueenPromoCap; flag >= KnightPromoCap; --flag) {
            moves.emplace_back(Move{from, to, flag});
        }
    }
    while (normal_captures) {
        const Square to = pop_lsb(normal_captures);
        const Square from = Square(to - (int)capture_dir);
        moves.emplace_back(Move{from, to, Capture});
    }
}

template <Side Us>
void generate_pawn_moves(Array<MoveScore, 256> &moves, const Pieces &pieces, const PositionState state) {
    constexpr Direction forward = (Us == White) ? North : South;
    constexpr Piece pawn = piece_of(Us, Pawn);

    const Bitboard empty = ~pieces.all();
    const Bitboard enemy = pieces.all_of(!Us);
    const Bitboard our_pawns = pieces.get(pawn);
    const Bitboard pushed_1 = empty & move_pawn<forward>(our_pawns);

    generate_single_pawn_pushes<Us>(moves, pushed_1);
    generate_double_pawn_pushes<Us>(moves, pushed_1, empty);
    generate_pawn_captures<Us, West>(moves, our_pawns, enemy);
    generate_pawn_captures<Us, East>(moves, our_pawns, enemy);
    if (state.has_en_passant()) {
        generate_en_croissants<Us>(moves, our_pawns, state.en_passant());
    }
}

} // namespace

Array<MoveScore, 256> pseudo_legals(const Position &position) {
    Array<MoveScore, 256> moves;
    const PositionState state = position.state();
    const Pieces &pieces = position.pieces();

    if (state.side_to_move() == White) {
        generate_pawn_moves<White>(moves, pieces, state);
        generate_non_pawn_moves<White>(moves, pieces);
        generate_castling_moves<White>(moves, pieces, state);
    } else {
        generate_pawn_moves<Black>(moves, pieces, state);
        generate_non_pawn_moves<Black>(moves, pieces);
        generate_castling_moves<Black>(moves, pieces, state);
    }

    return moves;
}

Array<Move, 256> legals(Position &position) {
    Array<Move, 256> moves;

    for (const auto [move, _] : pseudo_legals(position)) {
        if (position.try_do_pseudo(move)) {
            position.undo_move();
            moves.emplace_back(move);
        }
    }

    return moves;
}

} // namespace cheslib::movegen
