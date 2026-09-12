#pragma once

#include "assets.hpp"
#include "game_states.hpp"

namespace ches {

struct GameContext {
    Assets assets;
    cheslib::Engine engine;
    cheslib::Side user;
};

class Game {
  public:
    Game();
    void run();

  private:
    void update();
    void render() const;

  private:
    Window _window;
    GameState _state;
    GameContext _context;
};

} // namespace ches
