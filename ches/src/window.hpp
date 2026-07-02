#pragma once

#include <cstdint>
#include <memory>

#include <raylib.h>

namespace ches {

enum class KeyState : uint8_t {
    Idle,
    Pressed,
    Holding,
    Released
};

struct MouseEvent {
    ::Vector2 position;
    KeyState left;
};

/// manages game window, input polling and begin/end frame
class Window {
  public:
    Window(
        int width, int height, const char *title, ::ConfigFlags flags, ::Color background, std::shared_ptr<::Image> icon
    );

    bool should_close() const;
    MouseEvent poll_mouse() const;
    void set_cursor(::MouseCursor cursor);

    void update();
    void begin_frame() const;
    void end_frame() const;

    ~Window();
    Window(Window &&) = delete;
    Window(const Window &) = delete;
    Window &operator=(Window &&) = delete;
    Window &operator=(const Window &) = delete;

  private:
    const int _virtual_width;
    const int _virtual_height;
    const ::Color _background;

    ::Camera2D _camera;
    ::MouseCursor _cursor;
    std::shared_ptr<::Image> _icon;
};

} // namespace ches
