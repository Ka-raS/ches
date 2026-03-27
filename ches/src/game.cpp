#include <algorithm>

#include <raylib.h>

#include "config.hpp"
#include "game.hpp"
#include "cheslib/game_master.hpp"

namespace ches {

Game::Game() {
    cheslib::GameMaster gameMaster;
}

void Game::run() {
    while (!WindowShouldClose()) {
        _window.render();
    }
}

} // namespace ches
