#pragma once

namespace ches {

class Drawable {
  public:
    virtual ~Drawable();
    virtual void draw() const = 0;

  protected:
    Drawable() = default;
};

} // namespace ches
