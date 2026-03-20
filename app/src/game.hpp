#pragma once
#include "window.hpp"
#include "cheslib/board.hpp"

namespace ches {

class Game {
  public:
    Game();
    ~Game();
    void run();

  private:
    cheslib::Board _board;
    Window _window;
};

} // namespace ches
