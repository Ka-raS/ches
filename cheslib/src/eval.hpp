#pragma once

#include "position.hpp"

namespace cheslib::eval {

Score value_of(Piece piece);
Score value_of(PieceType type);
Score evaluate(const Position &position);

} // namespace cheslib::eval
