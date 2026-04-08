#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "cheslib/types.hpp"

namespace ches {

namespace detail {

struct MagicInfo {
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
extern const std::array<Bitboard, SquareCNT> WhitePawnAttacks;
extern const std::array<Bitboard, SquareCNT> BlackPawnAttacks;
extern const std::array<MagicInfo, SquareCNT> RookMagics;
extern const std::array<MagicInfo, SquareCNT> BishopMagics;
extern const std::array<Bitboard, 102400> RookAttacks;
extern const std::array<Bitboard, 5248> BishopAttacks;

} // namespace detail

inline Bitboard knight_attacks(Square from) {
    assert(from < SquareCNT);
    return detail::KnightAttacks[from];
}

inline Bitboard king_attacks(Square from) {
    assert(from < SquareCNT);
    return detail::KingAttacks[from];
}

inline Bitboard rook_attacks(Square from, Bitboard occupancy) {
    assert(from < SquareCNT);
    const auto &magic = detail::RookMagics[from];
    return detail::RookAttacks[magic.offset + magic.index(occupancy)];
}

inline Bitboard bishop_attacks(Square from, Bitboard occupancy) {
    assert(from < SquareCNT);
    const auto &magic = detail::BishopMagics[from];
    return detail::BishopAttacks[magic.offset + magic.index(occupancy)];
}

template <Side Us>
inline Bitboard pawn_attacks(Square from) {
    assert(from < SquareCNT);
    return (Us == White) ? detail::WhitePawnAttacks[from] : detail::BlackPawnAttacks[from];
}

inline Bitboard queen_attacks(Square from, Bitboard occupancy) {
    return bishop_attacks(from, occupancy) | rook_attacks(from, occupancy);
}

} // namespace ches
