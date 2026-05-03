#pragma once

#include <array>

#include "state.hpp"

namespace cheslib {

/**
 * Zobrist hash key
 * see: https://www.chessprogramming.org/Zobrist_Hashing
 */
using ZobristKey = uint64_t;

namespace zobrist {

ZobristKey hash(const std::array<Piece, SquareCNT> &board, State state);
ZobristKey piece(Piece piece, Square sq);
ZobristKey side();
ZobristKey en_passant(File file);
ZobristKey castling(CastleFlag flag);

} // namespace zobrist

} // namespace cheslib