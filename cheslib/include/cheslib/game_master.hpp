#pragma once

#include <vector>

#include "cheslib/move.hpp"
#include "cheslib/board.hpp"
#include "cheslib/attack_tables.hpp"
#include "cheslib/constants.hpp"

namespace cheslib {

class GameMaster {
  public:
    GameMaster() {
    }

    void reset() {
    }

    bool move(Move move) {
    }

    Board get_board() const {
        return _board;
    }

    void get_all_moves() const {
    }

    void get_best_move() const {
    }

  private:
    Board _board;
};

} // namespace cheslib
