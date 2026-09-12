#pragma once

#include <raylib.h>

namespace ches {

class Assets {
  public:
    Assets();
    const ::Font &font() const;
    const ::Image &icon() const;
    const ::Texture2D &pieces_sprite() const;

    ~Assets();
    Assets(Assets &&) = delete;
    Assets(const Assets &) = delete;
    Assets &operator=(Assets &&) = delete;
    Assets &operator=(const Assets &) = delete;

  private:
    ::Font _font;
    ::Image _icon;
    ::Texture2D _pieces_sprite;
};

} // namespace ches
