#pragma once

#include "types.hpp"

namespace cheslib::attacks {

Bitboard pawn(Square from, Side us);
Bitboard knight(Square from);
Bitboard king(Square from);
Bitboard rook(Square from, Bitboard occupancy);
Bitboard bishop(Square from, Bitboard occupancy);
Bitboard queen(Square from, Bitboard occupancy);

} // namespace cheslib::attacks