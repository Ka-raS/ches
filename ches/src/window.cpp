#include "window.hpp"
#include "config.hpp"

#include <algorithm>
#include <cassert>

namespace ches {

Window::Window() :
    _camera{.offset{0, 0}, .target{0, 0}, .rotation = 0, .zoom = 1},
    _cursor{::MOUSE_CURSOR_DEFAULT} {
    ::SetConfigFlags(config::WindowConfigs);
    ::InitWindow(config::WindowWidth, config::WindowHeight, config::GameTitle);
}

Window::~Window() {
    ::CloseWindow();
}

bool Window::should_close() const {
    return ::WindowShouldClose();
}

void Window::set_icon(const ::Image &icon) const {
    ::SetWindowIcon(icon);
}

void Window::set_cursor(const ::MouseCursor cursor) {
    if (cursor != _cursor) {
        _cursor = cursor;
        ::SetMouseCursor(cursor);
    }
}

void Window::update() {
    if (!::IsWindowResized()) {
        return;
    }

    const float width = (float)::GetScreenWidth();
    const float height = (float)::GetScreenHeight();
    const float scale = std::min(width / config::WindowWidth, height / config::WindowHeight);

    _camera.zoom = scale;
    _camera.offset.x = (width - config::WindowWidth * scale) / 2;
    _camera.offset.y = (height - config::WindowHeight * scale) / 2;
}

MouseEvent Window::poll_mouse() const {
    KeyState left;

    if (::IsMouseButtonReleased(::MOUSE_BUTTON_LEFT)) {
        left = KeyState::Released;
    } else if (::IsMouseButtonUp(::MOUSE_BUTTON_LEFT)) {
        left = KeyState::Idle;
    } else if (::IsMouseButtonPressed(::MOUSE_BUTTON_LEFT)) {
        left = KeyState::Pressed;
    } else {
        assert(::IsMouseButtonDown(::MOUSE_BUTTON_LEFT));
        left = KeyState::Holding;
    }

    return MouseEvent{::GetScreenToWorld2D(::GetMousePosition(), _camera), left};
}

void Window::begin_frame() const {
    ::BeginDrawing();
    ::ClearBackground(config::Background);
    ::BeginMode2D(_camera);
}

void Window::end_frame() const {
    ::EndMode2D();
    ::EndDrawing();
}

} // namespace ches
