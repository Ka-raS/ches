#include "window.hpp"

#include <cassert>
#include <cmath>

namespace ches {

Window::Window(
    const int width, const int height, const char *const title, const ::ConfigFlags flags, const ::Color background,
    std::shared_ptr<::Image> icon
) :
    _virtual_width{width},
    _virtual_height{height},
    _background{background},
    _camera{.offset{0, 0}, .target{0, 0}, .rotation = 0, .zoom = 1},
    _cursor{::MOUSE_CURSOR_DEFAULT},
    _icon{std::move(icon)} {

    ::SetConfigFlags(flags);
    ::InitWindow(width, height, title);
    ::SetWindowIcon(*_icon);
}

Window::~Window() {
    ::CloseWindow();
}

bool Window::should_close() const {
    return ::WindowShouldClose();
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
    const float scale = std::min(width / _virtual_width, height / _virtual_height);

    _camera.zoom = scale;
    _camera.offset.x = (width - _virtual_width * scale) / 2;
    _camera.offset.y = (height - _virtual_height * scale) / 2;
}

MouseEvent Window::poll_mouse() const {
    MouseEvent mouse{.position = ::GetScreenToWorld2D(::GetMousePosition(), _camera)};

    if (::IsMouseButtonReleased(::MOUSE_BUTTON_LEFT)) {
        mouse.left = KeyState::Released;
    } else if (::IsMouseButtonUp(::MOUSE_BUTTON_LEFT)) {
        mouse.left = KeyState::Idle;
    } else if (::IsMouseButtonPressed(::MOUSE_BUTTON_LEFT)) {
        mouse.left = KeyState::Pressed;
    } else {
        assert(::IsMouseButtonDown(::MOUSE_BUTTON_LEFT));
        mouse.left = KeyState::Holding;
    }

    return mouse;
}

void Window::begin_frame() const {
    ::BeginDrawing();
    ::ClearBackground(_background);
    ::BeginMode2D(_camera);
}

void Window::end_frame() const {
    ::EndMode2D();
    ::EndDrawing();
}

} // namespace ches
