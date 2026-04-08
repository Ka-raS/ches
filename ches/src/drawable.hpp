#pragma once

class Drawable {
  public:
    virtual ~Drawable();
    virtual void draw() const = 0;

  protected:
    Drawable() = default;
};
