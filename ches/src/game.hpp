#pragma once

#include "asset_manager.hpp"
#include "game_states.hpp"
#include "ui.hpp"

namespace ches {

struct GameContext {
    cheslib::Engine engine;
    BoardUI board_ui;
    UIPanel ui_panel;
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
    AssetManager _assets;
    Window _window;
    GameState _state;
    GameContext _context;
};

} // namespace ches
