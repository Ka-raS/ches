#pragma once

#include "cheslib/move.hpp"

#include "position.hpp"

namespace cheslib {

MoveList generate_pseudo_legals(const Position &position);

} // namespace cheslib
