#pragma once

#include <array>
#include <cassert>
#include <cstdint>

#include "cheslib/types.hpp"

#include "state.hpp"

namespace ches {

// Zobrist hash key
// see: https://www.chessprogramming.org/Zobrist_Hashing
using ZKey = uint64_t;

namespace detail {

extern const ZKey zobrist_side_key;
extern const std::array<ZKey, BothCastles + 1> zobrist_castling_keys;
extern const std::array<ZKey, FileCNT + 1> zobrist_en_passant_keys;
extern const std::array<ZKey, PieceCNT * (int)SquareCNT> zobrist_piece_keys;

} // namespace detail

ZKey zobrist_hash(const std::array<Piece, SquareCNT> &board, const State &state);

inline ZKey zobrist_piece(Piece piece, Square sq) {
    assert(piece < PieceCNT);
    assert(sq < SquareCNT);
    return detail::zobrist_piece_keys[piece * (int)SquareCNT + sq];
}

inline ZKey zobrist_side() {
    return detail::zobrist_side_key;
}

inline ZKey zobrist_en_passant(File file) {
    assert(file <= FileCNT);
    return detail::zobrist_en_passant_keys[file];
}

inline ZKey zobrist_castling(CastleFlag flag) {
    assert(flag <= BothCastles);
    return detail::zobrist_castling_keys[flag];
}

} // namespace ches