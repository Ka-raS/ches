#pragma once
#include "cheslib/bitboard.hpp"
#include "cheslib/constants.hpp"
#include "cheslib/gamestate.hpp"
#include "cheslib/pieces.hpp"

namespace cheslib {

class Board {
  public:
    Board();
    ~Board() = default;

    // bool move(Move move);
    void undo();

    // bool isCheck() const;
    // bool isCheckmate() const;
    // bool isStalemate() const;

    const GameState &state() const;
    const BitBoard &bitBoard() const;
    Piece pieceAt(size_t row, size_t col) const;

  private:
    GameState _state;
    BitBoard _bitBoard;
    Piece _squares[consts::BOARD_SIZE][consts::BOARD_SIZE];
};

} // namespace cheslib