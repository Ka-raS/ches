#pragma once

#include "drawable.hpp"

namespace ches {

class BoardView : public Drawable {
  public:
    BoardView(const cheslib::Board& board);
    ~BoardView() = default;
    void draw() const override;

  private:
    const cheslib::Board& _board;
};

} // namespace ches
