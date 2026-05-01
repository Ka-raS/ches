#pragma once

#include "types.hpp"

namespace cheslib::attacks {

template <Side Us>
Bitboard pawn(Square from);
Bitboard knight(Square from);
Bitboard king(Square from);
Bitboard rook(Square from, Bitboard occupancy);
Bitboard bishop(Square from, Bitboard occupancy);
Bitboard queen(Square from, Bitboard occupancy);

} // namespace cheslib::attacks