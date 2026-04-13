#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "cheslib/types.hpp"

namespace cheslib::attack_tables {

namespace detail {

struct Magic {
    Bitboard mask;
    uint64_t magic;
    uint32_t offset;
    uint8_t shift;

    constexpr size_t index(Bitboard occupancy) const {
        return ((occupancy & mask) * magic) >> (64 - shift);
    }
};

extern const std::array<Bitboard, SquareCNT> KnightAttacks;
extern const std::array<Bitboard, SquareCNT> KingAttacks;
extern const std::array<std::array<Bitboard, SquareCNT>, 2> PawnAttacks; // [Side][Square]
extern const std::array<Magic, SquareCNT> RookMagics;
extern const std::array<Magic, SquareCNT> BishopMagics;
extern const std::array<Bitboard, 102400> RookAttacks;
extern const std::array<Bitboard, 5248> BishopAttacks;

} // namespace detail

inline Bitboard knight(Square from) {
    assert(from < SquareCNT);
    return detail::KnightAttacks[from];
}

inline Bitboard king(Square from) {
    assert(from < SquareCNT);
    return detail::KingAttacks[from];
}

inline Bitboard rook(Square from, Bitboard occupancy) {
    assert(from < SquareCNT);
    const auto &magic = detail::RookMagics[from];
    return detail::RookAttacks[magic.offset + magic.index(occupancy)];
}

inline Bitboard bishop(Square from, Bitboard occupancy) {
    assert(from < SquareCNT);
    const auto &magic = detail::BishopMagics[from];
    return detail::BishopAttacks[magic.offset + magic.index(occupancy)];
}

inline Bitboard pawn(Square from, Side us) {
    assert(from < SquareCNT);
    return detail::PawnAttacks[us][from];
}

inline Bitboard queen(Square from, Bitboard occupancy) {
    return bishop(from, occupancy) | rook(from, occupancy);
}

} // namespace cheslib::attack_tables
