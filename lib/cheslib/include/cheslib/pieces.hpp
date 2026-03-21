#pragma once
#include <cstdint>

namespace cheslib
{

enum class PieceType : uint8_t {
    None = 0,
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

enum class PieceColor : bool {
    White,
    Black
};

struct Piece {
    PieceType type;
    PieceColor color;
};

constexpr char pieceTypeToChar(PieceType type) {
    switch (type) {
        case PieceType::None:   return '.';
        case PieceType::Pawn:   return 'P';
        case PieceType::Knight: return 'N';
        case PieceType::Bishop: return 'B';
        case PieceType::Rook:   return 'R';
        case PieceType::Queen:  return 'Q';
        case PieceType::King:   return 'K';
    }
}

} // namespace cheslib
