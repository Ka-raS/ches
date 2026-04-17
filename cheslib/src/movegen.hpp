#pragma once

#include "cheslib/move.hpp"

#include "pieces.hpp"
#include "state.hpp"

namespace cheslib::movegen {

MoveList pseudo_legals(const Pieces &pieces, State state);

} // namespace cheslib::movegen
