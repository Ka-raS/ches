#pragma once

#include "position.hpp"

namespace cheslib::movegen {

/// @param position is restored to original on return
void legals(Position &position, Array<Move, 256> &moves);

Array<MoveScore, 256> pseudo_legals(const Position &position);

} // namespace cheslib::movegen
