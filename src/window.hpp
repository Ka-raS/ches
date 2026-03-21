#pragma once
#include "drawable.hpp"
#include <vector>
#include <raylib.h>

namespace ches {

class Window {
  public:
    Window();
    ~Window();
    void render() const;
    void addDrawable(const Drawable & drawable);

  private:
    RenderTexture2D _renderTarget;
    std::vector<std::reference_wrapper<const Drawable>> _drawables;
};

} // namespace ches
