#pragma once
#include "window.hpp"
#include "cheslib/board.hpp"

namespace ches {

class Game {
  public:
    Game();
    ~Game() = default;
    void run();

  private:
    Window _window;
    cheslib::Board _board;
};

} // namespace ches
