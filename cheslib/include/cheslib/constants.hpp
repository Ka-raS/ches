#pragma once

namespace cheslib {

inline constexpr int BOARD_SIZE = 8;
inline constexpr int NUM_SQUARES = BOARD_SIZE * BOARD_SIZE;

// clang-format off
/**
 * Enum representing each square on the chessboard.
 */
enum Square : int {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NO_SQUARE = -1
};
// clang-format on

constexpr int to_square(int x, int y) {
    return y * BOARD_SIZE + x;
}

enum Direction : int {
    UP = to_square(0, 1),
    RIGHT = to_square(1, 0),
    DOWN = -UP,
    LEFT = -RIGHT,

    UP_RIGHT = UP + RIGHT,
    DOWN_RIGHT = DOWN + RIGHT,
    DOWN_LEFT = -UP_RIGHT,
    UP_LEFT = -DOWN_RIGHT
};

enum class Piece : int {
    NONE = 0,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

} // namespace cheslib
