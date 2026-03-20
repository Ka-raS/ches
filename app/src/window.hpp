#pragma once
#include <raylib.h>

namespace ches {

class Window {
  public:
    Window();
    ~Window();
    void render() const;

  private:

  private:
    RenderTexture2D _renderTarget;
};

} // namespace ches
