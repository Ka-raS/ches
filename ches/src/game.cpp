#include "game.hpp"
#include "config.hpp"

namespace ches {

namespace cl = ::cheslib;

Game::Game() :
    // clang-format off
    _assets{},
    _window{
        config::WindowWidth,
        config::WindowHeight,
        config::GameTitle,
        config::WindowConfigs,
        config::Background,
        _assets.load_image(config::IconPath)
    },
    _state{state::SelectingPiece{}},
    _context{
        .engine{
            config::EngineDepth,
            config::EngineThreadCount
        },
        .board_ui{
            config::SquareSize,
            config::BoardPos,
            config::DarkSquare,
            config::LightSquare,
            config::Highlight,
            config::MoveHint,
            cl::Side::White,
            _assets.load_font(config::FontPath),
            _assets.load_texture(config::PiecesSpritePath)
        },
        .ui_panel{
            config::UIPanelRect,
            config::PanelBackground,
            _assets.load_font(config::FontPath),
            _assets.load_texture(config::PiecesSpritePath)
        },
        .user{cl::Side::White}
    } // clang-format on
{
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
