#pragma once
#include <vector>
#include <raylib.h>
#include "drawable.hpp"

//nên có các hàm raylib draw wrapper để xử lý scaling 

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
