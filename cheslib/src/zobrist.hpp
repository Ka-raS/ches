#pragma once

#include <array>

#include "position_state.hpp"

namespace cheslib::zobrist {

ZobristKey hash(const std::array<Piece, SquareCNT> &board, PositionState state);
ZobristKey piece(Piece piece, Square square);
ZobristKey side();
ZobristKey en_passant(File file);
ZobristKey castling(CastleFlag flag);

} // namespace cheslib