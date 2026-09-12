#pragma once

#include <cstdint>

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
    Window();

    bool should_close() const;
    MouseEvent poll_mouse() const;
    void set_cursor(::MouseCursor cursor);
    void set_icon(const ::Image &icon) const;

    void update();
    void begin_frame() const;
    void end_frame() const;

    ~Window();
    Window(Window &&) = delete;
    Window(const Window &) = delete;
    Window &operator=(Window &&) = delete;
    Window &operator=(const Window &) = delete;

  private:
    ::Camera2D _camera;
    ::MouseCursor _cursor;
};

} // namespace ches
