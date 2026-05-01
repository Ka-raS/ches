#pragma once

#include "cheslib/array.hpp"
#include "cheslib/move.hpp"

#include "position.hpp"

namespace cheslib::movegen {

Array<Move, 256> pseudo_legals(const Position &position);

} // namespace cheslib::movegen
