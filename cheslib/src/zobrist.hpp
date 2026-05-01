#pragma once

#include <array>

#include "state.hpp"
#include "types.hpp"

namespace cheslib::zobrist {

ZKey hash(const std::array<Piece, SquareCNT> &board, State state);
ZKey piece(Piece piece, Square sq);
ZKey side();
ZKey en_passant(File file);
ZKey castling(CastleFlag flag);

} // namespace cheslib::zobrist