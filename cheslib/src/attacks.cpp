#include <array>
#include <span>

#include <immintrin.h>

#include "attacks.hpp"

namespace cheslib::attacks {

namespace {

struct Magic {
    Bitboard mask;
    std::size_t offset;

    std::size_t index(Bitboard occupancy) const;
    consteval std::size_t index_constexpr(Bitboard occupancy) const;
};

std::size_t Magic::index(Bitboard occupancy) const {
    return offset + _pext_u64(occupancy, mask);
}

consteval std::size_t Magic::index_constexpr(const Bitboard occupancy) const {
    uint64_t pext = 0;
    uint64_t bit = 1;
    Bitboard blockers = mask;

    while (blockers) {
        Bitboard lsb = blockers & -blockers;
        if (occupancy & lsb) {
            pext |= bit;
        }
        blockers &= blockers - 1;
        bit <<= 1;
    }
    return offset + pext;
}

consteval Square next_square(Square from, int8_t step) {
    // check rank wraparound
    Square to = Square(from + step);
    if (to >= SquareCNT) {
        return SquareCNT;
    }

    // check file wraparound
    int d_file = types::file_of(from) - types::file_of(to);
    if (-2 <= d_file && d_file <= 2) {
        return to;
    } else {
        return SquareCNT;
    }
}

// generate precomputed attack bitboards for knight/king/pawn
consteval std::array<Bitboard, SquareCNT> stepping_attacks(std::span<const int8_t> steps) {
    std::array<Bitboard, SquareCNT> result = {0};

    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        for (int8_t step : steps) {
            Square next = next_square(sq, step);
            if (next < SquareCNT) {
                types::set_square(result[sq], next);
            }
        }
    }

    return result;
}

// compute blocker mask for rook/bishop
consteval Bitboard sliding_blockers(const Square from, std::span<const Direction> directions) {
    Bitboard result = 0;

    for (Direction dir : directions) {
        Square curr = next_square(from, dir);
        if (curr >= SquareCNT) {
            continue;
        }

        // exclude edge squares
        Square next = next_square(curr, dir);
        while (next < SquareCNT) {
            types::set_square(result, curr);
            curr = next;
            next = next_square(curr, dir);
        }
    }

    return result;
}

consteval std::array<Magic, SquareCNT> magic_infos(std::span<const Direction> directions) {
    std::array<Magic, SquareCNT> result{};
    result[0] = Magic{.mask = sliding_blockers(Square(0), directions), .offset = 0};

    for (Square sq = Square(1); sq < SquareCNT; ++sq) {
        unsigned prev_mask_bits = std::popcount(result[sq - 1].mask);
        std::size_t prev_table_size = 1ULL << prev_mask_bits;

        std::size_t offset = result[sq - 1].offset + prev_table_size;
        Bitboard mask = sliding_blockers(sq, directions);

        result[sq] = Magic{mask, offset};
    }

    return result;
}

// compute attack bitboard for for rook/bishop
consteval Bitboard sliding_attack_at(
    const Square from, const Bitboard occupancy, std::span<const Direction> directions
) {
    Bitboard result = 0;

    for (Direction dir : directions) {
        Square curr = next_square(from, dir);

        while (curr < SquareCNT) {
            types::set_square(result, curr);
            bool is_blocked = types::has_square(occupancy, curr);
            if (is_blocked) {
                break;
            }
            curr = next_square(curr, dir);
        }
    }

    return result;
}

// generate magic bitboards for rook/bishop
template <std::size_t Size>
consteval std::array<Bitboard, Size> sliding_attacks(
    const std::array<Magic, SquareCNT> &magics, std::span<const Direction> directions
) {
    std::array<Bitboard, Size> attacks{};

    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        const Magic &magic = magics[sq];
        Bitboard occupancy = 0;

        // iterate all occupancy subsets
        // see: https://www.chessprogramming.org/Traversing_Subsets_of_a_Set
        do {
            std::size_t index = magic.index_constexpr(occupancy);
            attacks[index] = sliding_attack_at(sq, occupancy, directions);

            occupancy = (occupancy - magic.mask) & magic.mask;
        } while (occupancy);
    }

    return attacks;
}

constexpr int8_t KnightSteps[] = {NorthEast + North, NorthEast + East, SouthEast + East, SouthEast + South,
                                  SouthWest + South, SouthWest + West, NorthWest + West, NorthWest + North};
constexpr int8_t KingSteps[] = {North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest};
constexpr int8_t WhitePawnSteps[] = {NorthWest, NorthEast};
constexpr int8_t BlackPawnSteps[] = {SouthWest, SouthEast};
constexpr Direction RookDirections[] = {North, East, South, West};
constexpr Direction BishopDirections[] = {NorthEast, SouthEast, SouthWest, NorthWest};

constexpr std::array<Bitboard, SquareCNT> KnightAttacks = stepping_attacks(KnightSteps);
constexpr std::array<Bitboard, SquareCNT> KingAttacks = stepping_attacks(KingSteps);
constexpr std::array<Bitboard, SquareCNT> PawnAttacks[] = {
    stepping_attacks(WhitePawnSteps), stepping_attacks(BlackPawnSteps)
};

constexpr std::size_t RookSize = 102400;
constexpr std::size_t BishopSize = 5248;
constexpr std::array<Magic, SquareCNT> RookMagics = magic_infos(RookDirections);
constexpr std::array<Magic, SquareCNT> BishopMagics = magic_infos(BishopDirections);
constexpr std::array<Bitboard, RookSize> RookAttacks = sliding_attacks<RookSize>(RookMagics, RookDirections);
constexpr std::array<Bitboard, BishopSize> BishopAttacks = sliding_attacks<BishopSize>(BishopMagics, BishopDirections);

} // namespace

Bitboard pawn(Square from, Side us) {
    assert(from < SquareCNT);
    return PawnAttacks[us][from];
}

Bitboard knight(Square from) {
    assert(from < SquareCNT);
    return KnightAttacks[from];
}

Bitboard king(Square from) {
    assert(from < SquareCNT);
    return KingAttacks[from];
}

Bitboard rook(Square from, Bitboard occupancy) {
    assert(from < SquareCNT);
    std::size_t index = RookMagics[from].index(occupancy);
    return RookAttacks[index];
}

Bitboard bishop(Square from, Bitboard occupancy) {
    assert(from < SquareCNT);
    std::size_t index = BishopMagics[from].index(occupancy);
    return BishopAttacks[index];
}

Bitboard queen(Square from, Bitboard occupancy) {
    return bishop(from, occupancy) | rook(from, occupancy);
}

} // namespace cheslib::attacks
