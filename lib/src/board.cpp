#include "cheslib/board.hpp"
#include "cheslib/constants.hpp"

namespace cheslib {

Board::Board() {
    // use info from bitboard to initialize squares
    for (size_t row = 0; row < consts::BOARD_SIZE; row++) {
        for (size_t col = 0; col < consts::BOARD_SIZE; col++) {
            _squares[row][col] = _bitBoard.pieceAt(row, col);
        }
    }
}

// void Board::move(Move move) {}

void Board::undo() {
}

const GameState &Board::state() const {
    return _state;
}
inline const BitBoard &Board::bitBoard() const {
    return _bitBoard;
}
inline Piece Board::pieceAt(size_t row, size_t col) const {
    if (row < consts::BOARD_SIZE && col < consts::BOARD_SIZE) {
        return _squares[row][col];
    }
    return {PieceType::None};
}

} // namespace cheslib
