#pragma once

#include <cstdint>

#include "cheslib/constants.hpp"

namespace cheslib {

// clang-format off
/**
 * See: https://www.chessprogramming.org/Bitboards
 *
 * Bitboard representation of a chessboard using `uint64_t`.
 * Each `uint64_t` with its 64 bits represent 64 squares on chessboard
 * If bit=1 then a piece of that type is on the square
 */

class Position {

public:

    MoveList generate_pseudo_legal_moves() const;

private:

    uint64_t whitePawns   = (1ULL << A2) | (1ULL << B2) | (1ULL << C2) | (1ULL << D2) |
                            (1ULL << E2) | (1ULL << F2) | (1ULL << G2) | (1ULL << H2);
    uint64_t whiteKnights = (1ULL << B1) | (1ULL << G1);
    uint64_t whiteBishops = (1ULL << C1) | (1ULL << F1);
    uint64_t whiteRooks   = (1ULL << A1) | (1ULL << H1);
    uint64_t whiteQueen   = (1ULL << D1);
    uint64_t whiteKing    = (1ULL << E1);

    uint64_t blackPawns   = (1ULL << A7) | (1ULL << B7) | (1ULL << C7) | (1ULL << D7) |
                            (1ULL << E7) | (1ULL << F7) | (1ULL << G7) | (1ULL << H7);
    uint64_t blackKnights = (1ULL << B8) | (1ULL << G8);
    uint64_t blackBishops = (1ULL << C8) | (1ULL << F8);
    uint64_t blackRooks   = (1ULL << A8) | (1ULL << H8);
    uint64_t blackQueen   = (1ULL << D8);
    uint64_t blackKing    = (1ULL << E8);

    bool is_white_turn = true;

    bool can_white_short_castle = true;
    bool can_white_long_castle  = true;
    bool can_black_short_castle = true;
    bool can_black_long_castle  = true;

    // when a pawn moves two squares, set to the square behind the pawn
    int en_passant_target = NO_SQUARE;

    // count non pawn moves and non capture moves for 50-move rule to draw
    int halfmove_counter  = 0;
};
// clang-format on














constexpr int WHITE_PAWN = 0, WHITE_KNIGHT = 1, WHITE_BISHOP = 2, WHITE_ROOK = 3, WHITE_QUEEN = 4, WHITE_KING = 5;
constexpr int BLACK_PAWN = 6, BLACK_KNIGHT = 7, BLACK_BISHOP = 8, BLACK_ROOK = 9, BLACK_QUEEN = 10, BLACK_KING = 11;

uint64_t Board::pieces_of_type(int piece_idx) const {
    return pieces.bitboards[piece_idx];
}

void Board::add_move(MoveList& moves, int from, int to, int captured, int promotion, int flags) const {
    moves.add({from, to, -1, captured, promotion, flags});
}

void Board::generate_pawn_moves(MoveList& moves) const {
    const uint64_t us = (side == 0) ? pieces.allWhitePieces() : pieces.allBlackPieces();
    const uint64_t them = (side == 0) ? pieces.allBlackPieces() : pieces.allWhitePieces();
    const uint64_t all = pieces.allPieces();
    const uint64_t empty = ~all;

    const int forward = (side == 0) ? UP : DOWN;
    const int double_rank = (side == 0) ? 1 : 6;
    const int promo_rank = (side == 0) ? 6 : 1;
    const bool is_white = (side == 0);
    uint64_t pawns = pieces_of_type(is_white ? WHITE_PAWN : BLACK_PAWN);

    while (pawns) {
        int from = std::countr_zero(pawns);
        pawns &= pawns - 1;
        int from_y = from / BOARD_SIZE;

        // Single push
        int to = from + forward;
        if (to >= 0 && to < 64 && (empty & (1ULL << to))) {
            if (from_y == promo_rank) {
                for (int promo : {WHITE_QUEEN, WHITE_ROOK, WHITE_BISHOP, WHITE_KNIGHT})
                    add_move(moves, from, to, -1, promo, 8);
            } else {
                add_move(moves, from, to);
                // Double push
                if (from_y == double_rank) {
                    int to2 = from + 2 * forward;
                    if ((empty & (1ULL << to2))) {
                        add_move(moves, from, to2, -1, 0, 1);
                    }
                }
            }
        }

        // Captures
        uint64_t captures = get_pawn_attacks(from, is_white) & them;
        while (captures) {
            int to = std::countr_zero(captures);
            captures &= captures - 1;
            if (from_y == promo_rank) {
                for (int promo : {WHITE_QUEEN, WHITE_ROOK, WHITE_BISHOP, WHITE_KNIGHT})
                    add_move(moves, from, to, -1, promo, 8);
            } else {
                add_move(moves, from, to);
            }
        }

        // En passant
        if (ep_square != -1 && (get_pawn_attacks(from, is_white) & (1ULL << ep_square))) {
            add_move(moves, from, ep_square, -1, 0, 2);
        }
    }
}

void Board::generate_castling_moves(MoveList& moves) const {
    const uint64_t all = pieces.allPieces();
    if (side == 0) { // White
        if (castling & 1) // kingside
            if (!(all & (1ULL << F1)) && !(all & (1ULL << G1)))
                add_move(moves, E1, G1, -1, 0, 4);
        if (castling & 2) // queenside
            if (!(all & (1ULL << D1)) && !(all & (1ULL << C1)) && !(all & (1ULL << B1)))
                add_move(moves, E1, C1, -1, 0, 4);
    } else { // Black
        if (castling & 4) // kingside
            if (!(all & (1ULL << F8)) && !(all & (1ULL << G8)))
                add_move(moves, E8, G8, -1, 0, 4);
        if (castling & 8) // queenside
            if (!(all & ((1ULL << D8) | (1ULL << C8) | (1ULL << B8))))
                add_move(moves, E8, C8, -1, 0, 4);
    }
}

MoveList Board::generate_pseudo_legal_moves() const {
    MoveList moves;

    generate_pawn_moves(moves);

    const int piece_indices[] = {WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING};
    const int black_offs = 6;
    
    const uint64_t us = (side == 0) ? pieces.allWhitePieces() : pieces.allBlackPieces();
    const uint64_t all = pieces.allPieces();
    using AttackFunc = uint64_t(*)(int, uint64_t);
    const AttackFunc attacks[] = {
        get_knight_attacks_with_occ,   // knight
        get_bishop_attacks,            // bishop
        get_rook_attacks,              // rook
        get_queen_attacks,             // queen
        get_king_attacks_with_occ      // king
    };

    for (int i = 0; i < 5; ++i) {
        int idx = (side == 0) ? piece_indices[i] : piece_indices[i] + black_offs;
        uint64_t bb = pieces.bitboards[idx];

        while (bb) {
            int from = std::countr_zero(bb);
            bb &= bb - 1;
            uint64_t attacks = attacks[i](from, all) & ~us;
            while (attacks) {
                int to = std::countr_zero(attacks);
                attacks &= attacks - 1;
                add_move(moves, from, to);
            }
        }
    }

    generate_castling_moves(moves);

    return moves;
}



} // namespace cheslib
