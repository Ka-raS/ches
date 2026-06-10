#pragma once

#include "position.hpp"

namespace cheslib::evaluate {

Score material(Piece piece);
Score material(PieceType type);
Score positional(const Position &position);

} // namespace cheslib::evaluate
