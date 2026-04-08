#pragma once

#include <array>
#include <cassert>
#include <cstdint>

#include "cheslib/types.hpp"

#include "state_info.hpp"

namespace ches {

// Zobrist hash key
// see: https://www.chessprogramming.org/Zobrist_Hashing
using ZKey = uint64_t;

namespace detail {

extern const ZKey side_to_move_key;
extern const std::array<ZKey, 4> castling_keys;
extern const std::array<ZKey, FileCNT> en_passant_keys;
extern const std::array<std::array<ZKey, SquareCNT>, PieceCNT> piece_keys;

} // namespace detail

inline ZKey zobrist_piece(Piece piece, Square sq) {
    assert(piece < PieceCNT);
    assert(sq < SquareCNT);
    return detail::piece_keys[piece][sq];
}

inline ZKey zobrist_side_to_move() {
    return detail::side_to_move_key;
}

inline ZKey zobrist_en_passant(File file) {
    assert(file < FileCNT);
    return detail::en_passant_keys[file];
}

inline ZKey zobrist_castling(const StateInfo &state) {
    ZKey key = 0;
    const auto &keys = detail::castling_keys;

    if (state.can_short_castle<White>()) {
        key ^= keys[0];
    }
    if (state.can_long_castle<White>()) {
        key ^= keys[1];
    }
    if (state.can_short_castle<Black>()) {
        key ^= keys[2];
    }
    if (state.can_long_castle<Black>()) {
        key ^= keys[3];
    }

    return key;
}

inline ZKey zobrist_hash(const std::array<Piece, SquareCNT> &board, const StateInfo &state) {
    ZKey key = 0;

    for (Square sq = SquareA1; sq < SquareCNT; ++sq) {
        Piece piece = board[sq];
        if (piece != PieceCNT) { // only if square occupied
            key ^= detail::piece_keys[piece][sq];
        }
    }

    if (state.side_to_move() == Black) {
        key ^= detail::side_to_move_key;
    }

    key ^= zobrist_castling(state);
    key ^= zobrist_en_passant(state.ep_file());

    return key;
}

} // namespace ches