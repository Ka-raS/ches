#include <algorithm>
#include <raylib.h>
#include "config.hpp"
#include "game.hpp"

namespace ches {

Game::Game() {
    
}

void Game::run() {
    while (!WindowShouldClose()) {
        _window.render();
    }
}

} // namespace ches
