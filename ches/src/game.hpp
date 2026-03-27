#pragma once
#include "window.hpp"

namespace ches {

class Game {
  public:
    Game();
    ~Game() = default;
    void run();

  private:
    Window _window;
};

} // namespace ches
