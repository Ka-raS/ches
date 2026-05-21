#pragma once

#include "position.hpp"

namespace cheslib::movegen {

Array<Move, 256> legals(Position &position);
Array<MoveScore, 256> pseudo_legals(const Position &position);

} // namespace cheslib::movegen
