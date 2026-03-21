#include "cheslib/bitboard.hpp"
#include "cheslib/constants.hpp"
#include "cheslib/pieces.hpp"
#include "cheslib/square.hpp"

namespace cheslib
{

BitBoard::BitBoard()
    : whitePawns(  (1ULL << sq::a2) | (1ULL << sq::b2) | (1ULL << sq::c2) | (1ULL << sq::d2) |
                   (1ULL << sq::e2) | (1ULL << sq::f2) | (1ULL << sq::g2) | (1ULL << sq::h2)),
      whiteKnights((1ULL << sq::b1) | (1ULL << sq::g1)),
      whiteBishops((1ULL << sq::c1) | (1ULL << sq::f1)),
      whiteRooks  ((1ULL << sq::a1) | (1ULL << sq::h1)),
      whiteQueen  ((1ULL << sq::d1)),
      whiteKing    (1ULL << sq::e1),
      blackPawns  ((1ULL << sq::a7) | (1ULL << sq::b7) | (1ULL << sq::c7) | (1ULL << sq::d7) |
                   (1ULL << sq::e7) | (1ULL << sq::f7) | (1ULL << sq::g7) | (1ULL << sq::h7)),
      blackKnights((1ULL << sq::b8) | (1ULL << sq::g8)),
      blackBishops((1ULL << sq::c8) | (1ULL << sq::f8)),
      blackRooks  ((1ULL << sq::a8) | (1ULL << sq::h8)),
      blackQueen  ((1ULL << sq::d8)),
      blackKing   ((1ULL << sq::e8))
{}

Piece BitBoard::pieceAt(size_t row, size_t col) const {
    size_t square = row * consts::BOARD_SIZE + col;
    uint64_t mask = 1ULL << square;

    if (whitePawns   & mask) return {PieceType::Pawn,   PieceColor::White};
    if (whiteKnights & mask) return {PieceType::Knight, PieceColor::White};
    if (whiteBishops & mask) return {PieceType::Bishop, PieceColor::White};
    if (whiteRooks   & mask) return {PieceType::Rook,   PieceColor::White};
    if (whiteQueen   & mask) return {PieceType::Queen,  PieceColor::White};
    if (whiteKing    & mask) return {PieceType::King,   PieceColor::White};

    if (blackPawns   & mask) return {PieceType::Pawn,   PieceColor::Black};
    if (blackKnights & mask) return {PieceType::Knight, PieceColor::Black};
    if (blackBishops & mask) return {PieceType::Bishop, PieceColor::Black};
    if (blackRooks   & mask) return {PieceType::Rook,   PieceColor::Black};
    if (blackQueen   & mask) return {PieceType::Queen,  PieceColor::Black};
    if (blackKing    & mask) return {PieceType::King,   PieceColor::Black};

    return {PieceType::None}; // No piece on this square
}

} // namespace cheslib
