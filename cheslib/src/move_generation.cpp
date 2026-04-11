#include "move_generation.hpp"
#include "attack_tables.hpp"

namespace ches {

namespace {

Bitboard non_pawn_attacks(PieceType type, Square from, Bitboard occupancy) {
    switch (type) {
    case Knight:
        return knight_attacks(from);
    case Bishop:
        return bishop_attacks(from, occupancy);
    case Rook:
        return rook_attacks(from, occupancy);
    case Queen:
        return queen_attacks(from, occupancy);
    case King:
        return king_attacks(from);
    default:
        return 0;
    }
}

template <Side Us>
void generate_non_pawn_moves(MoveList &moves, const PieceBitboards &pieces) {
    const Bitboard not_us = ~pieces.all<Us>();
    const Bitboard all = pieces.all();

    for (PieceType type = Knight; type < PieceTypeCNT; ++type) {
        Bitboard bb = pieces.get<Us>(type);
        while (bb) {
            Square from = pop_lsb(bb);
            Bitboard attacks = not_us & non_pawn_attacks(type, from, all);

            while (attacks) {
                Square to = pop_lsb(attacks);
                MoveFlag flag = has_square(all, to) ? Capture : QuietMove;
                moves.add(from, to, flag);
            }
        }
    }
}

template <Side Us>
void generate_castling_moves(MoveList &moves, const PieceBitboards &pieces, State state) {
    const Bitboard all = pieces.all();

    if constexpr (Us == White) {
        constexpr Bitboard short_blockers = to_bitboard(SquareF1, SquareG1);
        constexpr Bitboard long_blockers = to_bitboard(SquareD1, SquareC1, SquareB1);

        if (state.can_castles(WhiteShortCastles) && !(all & short_blockers)) {
            moves.add(SquareE1, SquareG1, ShortCastle);
        }
        if (state.can_castles(WhiteLongCastles) && !(all & long_blockers)) {
            moves.add(SquareE1, SquareC1, LongCastle);
        }

    } else {
        constexpr Bitboard short_blockers = to_bitboard(SquareF8, SquareG8);
        constexpr Bitboard long_blockers = to_bitboard(SquareD8, SquareC8, SquareB8);

        if (state.can_castles(BlackShortCastles) && !(all & short_blockers)) {
            moves.add(SquareE8, SquareG8, ShortCastle);
        }
        if (state.can_castles(BlackLongCastles) && !(all & long_blockers)) {
            moves.add(SquareE8, SquareC8, LongCastle);
        }
    }
}

// white pawn direction is > 0, black direction is < 0
template <Direction Dir>
constexpr Bitboard move_pawn(Bitboard bb) {
    if constexpr (Dir > 0) {
        return bb << Dir;
    } else {
        return bb >> -Dir;
    }
};

template <Side Us>
void generate_single_pawn_pushes(MoveList &moves, Bitboard pushed_1) {
    constexpr Bitboard promo_bb = (Us == White) ? rank_bitboard(Rank8) : rank_bitboard(Rank1);

    Bitboard promo_push = pushed_1 & promo_bb;
    Bitboard normal_push = pushed_1 & ~promo_bb;

    while (promo_push) {
        Square to = pop_lsb(promo_push);
        Square from = square_behind<Us>(to);
        for (MoveFlag flag = QueenPromo; flag >= KnightPromo; --flag) {
            moves.add(from, to, flag);
        }
    }
    while (normal_push) {
        Square to = pop_lsb(normal_push);
        Square from = square_behind<Us>(to);
        moves.add(from, to, QuietMove);
    }
}

template <Side Us>
void generate_double_pawn_pushes(MoveList &moves, Bitboard pushed_1, Bitboard empty) {
    constexpr Direction forward = (Us == White) ? Up : Down;
    constexpr Bitboard destination = (Us == White) ? rank_bitboard(Rank4) : rank_bitboard(Rank5);

    Bitboard pushed_2 = empty & destination & move_pawn<forward>(pushed_1);
    while (pushed_2) {
        Square to = pop_lsb(pushed_2);
        Square from = Square(to - 2 * forward);
        moves.add(from, to, DoublePawnPush);
    }
}

template <Side Us>
void generate_en_croissants(MoveList &moves, Bitboard our_pawns, File ep_file) {
    if (ep_file >= FileCNT) {
        return;
    }
    const Square ep_square = to_square(ep_file, (Us == White) ? Rank6 : Rank3);

    // enemy at Square ep_square attack us <=> us attack enemy at Square en_passant
    const Bitboard us_attacked = pawn_attacks<Side(!Us)>(ep_square);
    Bitboard our_attackers = our_pawns & us_attacked;

    while (our_attackers) {
        Square from = pop_lsb(our_attackers);
        moves.add(from, ep_square, EnPassant);
    }
}

template <Side Us, Direction Dir>
void generate_pawn_captures(MoveList &moves, Bitboard our_pawns, Bitboard enemy) {
    assert(Dir == Right || Dir == Left);

    constexpr bool is_white = Us == White;
    constexpr Direction capture_dir = Direction(Dir + (is_white ? Up : Down));
    constexpr Bitboard promo_bb = is_white ? rank_bitboard(Rank8) : rank_bitboard(Rank1);
    constexpr Bitboard targets = (Dir == Right) ? ~file_bitboard(FileA) : ~file_bitboard(FileH);

    const Bitboard captures = enemy & targets & move_pawn<capture_dir>(our_pawns);
    Bitboard normal_captures = captures & ~promo_bb;
    Bitboard promo_captures = captures & promo_bb;

    while (promo_captures) {
        Square to = pop_lsb(promo_captures);
        Square from = Square(to - (int)capture_dir);
        for (MoveFlag flag = QueenPromoCap; flag >= KnightPromoCap; --flag) {
            moves.add(from, to, flag);
        }
    }
    while (normal_captures) {
        Square to = pop_lsb(normal_captures);
        Square from = Square(to - (int)capture_dir);
        moves.add(from, to, Capture);
    }
}

template <Side Us>
void generate_pawn_moves(MoveList &moves, const PieceBitboards &pieces, State state) {
    constexpr bool is_white = (Us == White);
    constexpr Direction forward = (Us == White) ? Up : Down;

    const Bitboard empty = ~pieces.all();
    const Bitboard enemy = pieces.all<Side(!Us)>();
    const Bitboard our_pawns = pieces.get<Us>(Pawn);
    const Bitboard pushed_1 = empty & move_pawn<forward>(our_pawns);

    generate_single_pawn_pushes<Us>(moves, pushed_1);
    generate_double_pawn_pushes<Us>(moves, pushed_1, empty);
    generate_pawn_captures<Us, Left>(moves, our_pawns, enemy);
    generate_pawn_captures<Us, Right>(moves, our_pawns, enemy);
    generate_en_croissants<Us>(moves, our_pawns, state.en_passant());
}

} // namespace

MoveList generate_pseudo_legals(const Position &position) {
    MoveList moves{};
    State state = position.state();
    const PieceBitboards &pieces = position.pieces();

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

} // namespace ches
