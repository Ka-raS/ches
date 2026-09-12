#include "game.hpp"
#include "config.hpp"

namespace ches {

namespace cl = ::cheslib;

Game::Game() :
    _window{},
    _state{state::SelectingPiece{}},
    _context{
        .assets{}, //
        .engine{config::EngineDepth, config::EngineThreadCount},
        .user{cl::Side::White}
    } {
    _window.set_icon(_context.assets.icon());
    ::SetTargetFPS(config::FPSTarget);
    ::EnableEventWaiting();
};

void Game::run() {
    while (!_window.should_close()) {
        update();
        render();
    }
}

void Game::update() {
    _window.update();

    const auto [next_state, cursor] = std::visit([this](const auto &state) -> StateResult {
        return state.update(_context, _window.poll_mouse());
    }, _state);

    _state = next_state;
    _window.set_cursor(cursor);
}

void Game::render() const {
    _window.begin_frame();

    std::visit([this](const auto &state) -> void {
        state.draw(_context);
    }, _state);

    _window.end_frame();
}

} // namespace ches
