#pragma once

#include <array>
#include <bit>
#include <cstddef>

#include "cheslib/types.hpp"
#include "utils.hpp"

namespace cheslib::detail {

std::array<uint64_t, SQUARE_CNT> magic_numbers(
    const std::array<Bitboard, SQUARE_CNT> &masks, const std::array<uint8_t, SQUARE_CNT> &shifts,
    const std::array<Direction, 4> &directions
);

constexpr size_t magic_index(Bitboard occupancy, Bitboard mask, uint64_t magic, uint8_t shift) {
    return ((occupancy & mask) * magic) >> (64 - shift);
}

constexpr std::array<uint8_t, SQUARE_CNT> popcounts(const std::array<Bitboard, SQUARE_CNT> &masks) {
    std::array<uint8_t, SQUARE_CNT> result = {0};

    for (Square sq = SQUARE_A1; sq < SQUARE_CNT; ++sq) {
        result[sq] = std::popcount(masks[sq]);
    }

    return result;
}

constexpr std::array<uint32_t, SQUARE_CNT> occupancy_offsets(const std::array<uint8_t, SQUARE_CNT> &shifts) {
    std::array<uint32_t, SQUARE_CNT> result = {0};

    for (Square sq = Square(1); sq < SQUARE_CNT; ++sq) {
        uint32_t prev_size = 1U << shifts[sq - 1];
        result[sq] = result[sq - 1] + prev_size;
    }

    return result;
}

constexpr std::array<Bitboard, SQUARE_CNT> sliding_blockers(const std::array<Direction, 4> &directions) {
    std::array<Bitboard, SQUARE_CNT> result = {0};

    for (Square sq = SQUARE_A1; sq < SQUARE_CNT; ++sq) {
        for (Direction dir : directions) {
            Square curr = shift_square(sq, dir);
            if (curr >= SQUARE_CNT) {
                continue;
            }

            // exclude edge squares
            Square next = shift_square(curr, dir);
            while (next < SQUARE_CNT) {
                set_square(result[sq], curr);
                curr = next;
                next = shift_square(curr, dir);
            }
        }
    }

    return result;
}

constexpr Bitboard sliding_attack_at(Square from, Bitboard occupancy, const std::array<Direction, 4> &directions) {
    Bitboard result = 0;

    for (Direction dir : directions) {
        Square curr = shift_square(from, dir);

        while (curr < SQUARE_CNT) {
            set_square(result, curr);
            bool is_blocked = has_square(occupancy, curr);
            if (is_blocked) {
                break;
            }
            curr = shift_square(curr, dir);
        }
    }

    return result;
}

constexpr std::array<Square, SQUARE_CNT> blocker_positions(Bitboard mask) {
    std::array<Square, SQUARE_CNT> positions{};
    positions.fill(SQUARE_CNT);
    size_t count = 0;

    for (Square sq = SQUARE_A1; sq < SQUARE_CNT; ++sq) {
        if (has_square(mask, sq)) {
            positions[count++] = sq;
        }
    }

    return positions;
}

constexpr Bitboard occupancy_from_subset(
    size_t subset, size_t blocker_cnt, const std::array<Square, SQUARE_CNT> &blocker_pos
) {
    Bitboard occupancy = 0;

    for (size_t i = 0; i < blocker_cnt; ++i) {
        if (blocker_pos[i] >= SQUARE_CNT) {
            continue;
        }

        bool is_in_subset = subset & (1U << i);
        if (is_in_subset) {
            set_square(occupancy, blocker_pos[i]);
        }
    }

    return occupancy;
}

template <size_t N>
constexpr std::array<Bitboard, N> sliding_attacks(
    const std::array<Bitboard, SQUARE_CNT> &masks, const std::array<uint64_t, SQUARE_CNT> &magics,
    const std::array<uint8_t, SQUARE_CNT> &shifts, const std::array<uint32_t, SQUARE_CNT> &offsets,
    const std::array<Direction, 4> &directions
) {
    std::array<Bitboard, N> attacks = {0};

    for (Square sq = SQUARE_A1; sq < SQUARE_CNT; ++sq) {
        const size_t blocker_cnt = shifts[sq];
        const size_t subset_cnt = 1U << blocker_cnt;
        const auto blocker_pos = blocker_positions(masks[sq]);

        // iterate all occupancy subsets
        for (size_t subset = 0; subset < subset_cnt; ++subset) {
            Bitboard occupancy = occupancy_from_subset(subset, blocker_cnt, blocker_pos);
            size_t index = magic_index(occupancy, masks[sq], magics[sq], shifts[sq]);

            attacks[offsets[sq] + index] = sliding_attack_at(sq, occupancy, directions);
        }
    }

    return attacks;
}

template <size_t N>
constexpr std::array<Bitboard, SQUARE_CNT> stepping_attacks(const std::array<int8_t, N> &steps) {
    std::array<Bitboard, SQUARE_CNT> result = {0};

    for (Square sq = SQUARE_A1; sq < SQUARE_CNT; ++sq) {
        for (int8_t step : steps) {
            Square next = shift_square(sq, step);
            if (next < SQUARE_CNT) {
                set_square(result[sq], next);
            }
        }
    }

    return result;
}

} // namespace cheslib::detail
