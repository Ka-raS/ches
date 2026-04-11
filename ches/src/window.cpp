#include "window.hpp"
#include "config.hpp"
#include <algorithm>
#include <raylib.h>

Window::Window() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(config::WINDOW_WIDTH, config::WINDOW_HEIGHT, config::WINDOW_TITLE);
    EnableEventWaiting();
    _renderTarget = LoadRenderTexture(config::WINDOW_WIDTH, config::WINDOW_HEIGHT);
    _drawables.reserve(5);
}

Window::~Window() {
    UnloadRenderTexture(_renderTarget);
    CloseWindow();
}

void Window::addDrawable(const Drawable &drawable) {
    _drawables.emplace_back(drawable);
}

void Window::render() const {
    // Draw to render target
    BeginTextureMode(_renderTarget);
    ClearBackground(config::BACKGROUND_COLOR);
    for (const auto &drawable : _drawables) {
        drawable.get().draw();
    }
    EndTextureMode();

    // Draw the scaled render target to screen
    BeginDrawing();
    ClearBackground(config::BACKGROUND_COLOR);

    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    float scale = std::min(screenWidth / config::WINDOW_WIDTH, screenHeight / config::WINDOW_HEIGHT);
    float destWeight = config::WINDOW_WIDTH * scale;
    float destHeight = config::WINDOW_HEIGHT * scale;

    constexpr Rectangle source = {.x = 0, .y = 0, .width = config::WINDOW_WIDTH, .height = -config::WINDOW_HEIGHT};
    Rectangle dest = {
        .x = (screenWidth - destWeight) / 2,
        .y = (screenHeight - destHeight) / 2,
        .width = destWeight,
        .height = destHeight
    };

    DrawTexturePro(_renderTarget.texture, source, dest, {0, 0}, 0.0f, WHITE);
    EndDrawing();
}
