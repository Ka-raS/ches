#pragma once

#include "cheslib/move_list.hpp"

#include "position.hpp"

namespace cheslib::movegen {

MoveList legals(Position &position);
MoveList pseudo_legals(const Position &position);

} // namespace cheslib::movegen
