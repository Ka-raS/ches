#pragma once

#include "cheslib/move.hpp"

#include "position.hpp"

namespace ches {

MoveList generate_pseudo_legals(const Position &position);

} // namespace ches
