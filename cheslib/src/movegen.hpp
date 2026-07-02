#pragma once

#include "position.hpp"

namespace cheslib::movegen {

/// @param position is restored to original on return
Array<Move, 256> legals(Position &position);

Array<MoveScore, 256> pseudo_legals(const Position &position);

} // namespace cheslib::movegen
