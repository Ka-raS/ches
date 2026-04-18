#include "movegen.hpp"
#include "attacks.hpp"

namespace cheslib::movegen {

namespace {

Bitboard non_pawn_attacks(PieceType type, Square from, Bitboard occupancy) {
    switch (type) {
    case Knight:
        return attacks::knight(from);
    case Bishop:
        return attacks::bishop(from, occupancy);
    case Rook:
        return attacks::rook(from, occupancy);
    case Queen:
        return attacks::queen(from, occupancy);
    case King:
        return attacks::king(from);
    default:
        return 0;
    }
}

template <Side Us>
void generate_non_pawn_moves(MoveList &moves, const Pieces &pieces) {
    const Bitboard not_us = ~pieces.all<Us>();
    const Bitboard all = pieces.all();

    for (PieceType type = Knight; type <= King; ++type) {
        Bitboard bb = pieces.get<Us>(type);
        while (bb) {
            Square from = utils::pop_lsb(bb);
            Bitboard attacks = not_us & non_pawn_attacks(type, from, all);

            while (attacks) {
                Square to = utils::pop_lsb(attacks);
                MoveFlag flag = utils::has_square(all, to) ? Capture : QuietMove;
                moves.add(from, to, flag);
            }
        }
    }
}

template <Side Us>
void generate_castling_moves(MoveList &moves, const Pieces &pieces, State state) {
    const Bitboard all = pieces.all();

    if constexpr (Us == White) {
        constexpr Bitboard short_blockers = utils::bitboard_of(SquareF1, SquareG1);
        constexpr Bitboard long_blockers = utils::bitboard_of(SquareD1, SquareC1, SquareB1);

        if (state.can_castles(WhiteShortCastles) && !(all & short_blockers)) {
            moves.add(SquareE1, SquareG1, ShortCastle);
        }
        if (state.can_castles(WhiteLongCastles) && !(all & long_blockers)) {
            moves.add(SquareE1, SquareC1, LongCastle);
        }

    } else {
        constexpr Bitboard short_blockers = utils::bitboard_of(SquareF8, SquareG8);
        constexpr Bitboard long_blockers = utils::bitboard_of(SquareD8, SquareC8, SquareB8);

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
    constexpr Bitboard promo_bb = (Us == White) ? utils::bitboard_of(Rank8) : utils::bitboard_of(Rank1);

    Bitboard promo_push = pushed_1 & promo_bb;
    Bitboard normal_push = pushed_1 & ~promo_bb;

    while (promo_push) {
        Square to = utils::pop_lsb(promo_push);
        Square from = utils::square_behind<Us>(to);
        for (MoveFlag flag = QueenPromo; flag >= KnightPromo; --flag) {
            moves.add(from, to, flag);
        }
    }
    while (normal_push) {
        Square to = utils::pop_lsb(normal_push);
        Square from = utils::square_behind<Us>(to);
        moves.add(from, to, QuietMove);
    }
}

template <Side Us>
void generate_double_pawn_pushes(MoveList &moves, Bitboard pushed_1, Bitboard empty) {
    constexpr Direction forward = (Us == White) ? North : South;
    constexpr Bitboard destination = (Us == White) ? utils::bitboard_of(Rank4) : utils::bitboard_of(Rank5);

    Bitboard pushed_2 = empty & destination & move_pawn<forward>(pushed_1);
    while (pushed_2) {
        Square to = utils::pop_lsb(pushed_2);
        Square from = Square(to - 2 * forward);
        moves.add(from, to, DoublePawnPush);
    }
}

template <Side Us>
void generate_en_croissants(MoveList &moves, Bitboard our_pawns, File ep_file) {
    assert(ep_file < FileCNT);
    const Square ep_square = utils::square_of(ep_file, (Us == White) ? Rank6 : Rank3);

    // enemy at Square ep_square attack us <=> us attack enemy at Square en_passant
    const Bitboard us_attacked = attacks::pawn<Side(!Us)>(ep_square);
    Bitboard our_attackers = our_pawns & us_attacked;

    while (our_attackers) {
        Square from = utils::pop_lsb(our_attackers);
        moves.add(from, ep_square, EnPassant);
    }
}

template <Side Us, Direction Dir>
void generate_pawn_captures(MoveList &moves, Bitboard our_pawns, Bitboard enemy) {
    assert(Dir == East || Dir == West);

    constexpr bool is_white = Us == White;
    constexpr Direction capture_dir = Direction(Dir + (is_white ? North : South));
    constexpr Bitboard promo_bb = is_white ? utils::bitboard_of(Rank8) : utils::bitboard_of(Rank1);
    constexpr Bitboard mask = (Dir == East) ? ~utils::bitboard_of(FileA) : ~utils::bitboard_of(FileH);

    const Bitboard captures = enemy & mask & move_pawn<capture_dir>(our_pawns);
    Bitboard normal_captures = captures & ~promo_bb;
    Bitboard promo_captures = captures & promo_bb;

    while (promo_captures) {
        Square to = utils::pop_lsb(promo_captures);
        Square from = Square(to - (int)capture_dir);
        for (MoveFlag flag = QueenPromoCap; flag >= KnightPromoCap; --flag) {
            moves.add(from, to, flag);
        }
    }
    while (normal_captures) {
        Square to = utils::pop_lsb(normal_captures);
        Square from = Square(to - (int)capture_dir);
        moves.add(from, to, Capture);
    }
}

template <Side Us>
void generate_pawn_moves(MoveList &moves, const Pieces &pieces, State state) {
    constexpr bool is_white = (Us == White);
    constexpr Direction forward = (Us == White) ? North : South;

    const Bitboard empty = ~pieces.all();
    const Bitboard enemy = pieces.all<Side(!Us)>();
    const Bitboard our_pawns = pieces.get<Us>(Pawn);
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

MoveList pseudo_legals(const Pieces &pieces, State state) {
    MoveList moves{};

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

} // namespace cheslib::movegen
