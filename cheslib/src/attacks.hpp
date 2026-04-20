#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "cheslib/types.hpp"

#include "utils.hpp"

namespace cheslib::attacks {

template <Side Us>
inline Bitboard pawn(Square from);
inline Bitboard knight(Square from);
inline Bitboard king(Square from);
inline Bitboard rook(Square from, Bitboard occupancy);
inline Bitboard bishop(Square from, Bitboard occupancy);
inline Bitboard queen(Square from, Bitboard occupancy);

// implementation

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
extern const std::array<Bitboard, SquareCNT> WhitePawnAttacks;
extern const std::array<Bitboard, SquareCNT> BlackPawnAttacks;
extern const std::array<Magic, SquareCNT> RookMagics;
extern const std::array<Magic, SquareCNT> BishopMagics;
extern const std::array<Bitboard, 102400> RookAttacks;
extern const std::array<Bitboard, 5248> BishopAttacks;

} // namespace detail

template <Side Us>
inline Bitboard pawn(Square from) {
    assert(from < SquareCNT);
    return (Us == White) ? detail::WhitePawnAttacks[from] : detail::BlackPawnAttacks[from];
}

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
    const detail::Magic &magic = detail::RookMagics[from];
    return detail::RookAttacks[magic.offset + magic.index(occupancy)];
}

inline Bitboard bishop(Square from, Bitboard occupancy) {
    assert(from < SquareCNT);
    const detail::Magic &magic = detail::BishopMagics[from];
    return detail::BishopAttacks[magic.offset + magic.index(occupancy)];
}

inline Bitboard queen(Square from, Bitboard occupancy) {
    return bishop(from, occupancy) | rook(from, occupancy);
}

} // namespace cheslib::attacks
