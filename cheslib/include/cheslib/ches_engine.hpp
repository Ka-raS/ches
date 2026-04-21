#pragma once

#include <vector>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

namespace cheslib {

class ChesEngine {
  public:
    ChesEngine() {
    }

    void reset() {
    }

    bool move(Move move) {
        return true;
    }

    // Board get_board() const {
    //     return _board;
    // }

    void get_all_moves() const {
    }

    void get_best_move() const {
    }

  private:
    // Board _board;
};

} // namespace cheslib
