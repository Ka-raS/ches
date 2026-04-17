#pragma once

#include <array>
#include <cassert>
#include <cstdint>

#include "cheslib/types.hpp"

#include "state.hpp"

namespace cheslib {

// Zobrist hash key
// see: https://www.chessprogramming.org/Zobrist_Hashing
using ZKey = uint64_t;

namespace zobrist {

ZKey hash(const std::array<Piece, SquareCNT> &board, const State &state);
inline ZKey piece(Piece piece, Square sq);
inline ZKey side();
inline ZKey en_passant(File file);
inline ZKey castling(CastleFlag flag);

namespace detail {

extern const ZKey side_key;
extern const std::array<ZKey, BothCastles + 1> castling_keys;
extern const std::array<ZKey, FileCNT + 1> en_passant_keys;
extern const std::array<ZKey, PieceCNT *(int)SquareCNT> piece_keys;

} // namespace detail

inline ZKey piece(Piece piece, Square sq) {
    assert(piece < PieceCNT);
    assert(sq < SquareCNT);
    return detail::piece_keys[piece * (int)SquareCNT + sq];
}

inline ZKey side() {
    return detail::side_key;
}

inline ZKey en_passant(File file) {
    assert(file <= FileCNT);
    return detail::en_passant_keys[file];
}

inline ZKey castling(CastleFlag flag) {
    assert(flag <= BothCastles);
    return detail::castling_keys[flag];
}

} // namespace zobrist

} // namespace cheslib