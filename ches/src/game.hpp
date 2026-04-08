#pragma once
#include "window.hpp"

class Game {
  public:
    Game();
    ~Game() = default;
    void run();

  private:
    Window _window;
};
