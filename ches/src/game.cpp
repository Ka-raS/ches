#include <algorithm>

#include <raylib.h>

#include "config.hpp"
#include "game.hpp"

Game::Game() {
}

void Game::run() {
    while (!WindowShouldClose()) {
        _window.render();
    }
}
