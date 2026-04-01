#include <random>

#include "attack_tables.hpp"

namespace cheslib::detail {

namespace {

Bitboard find_magic(
    Square from, Bitboard mask, uint8_t shift, const std::array<Direction, 4> &directions, std::mt19937_64 &rng
) {
    // find all occupancies and attacks
    Bitboard occupancies[1 << 12] = {0};
    Bitboard attacks[1 << 12] = {0};
    const size_t subset_cnt = 1U << shift;
    std::array<Square, 12> blocker_squares = blockers_from_mask(mask);

    for (size_t subset = 0; subset < subset_cnt; ++subset) {
        occupancies[subset] = occupancy_from_subset(subset, shift, blocker_squares);
        attacks[subset] = sliding_attack_at(from, occupancies[subset], directions);
    }

    while (true) {
        const Bitboard magic = rng() & rng() & rng();

        bool has_enough_bits = std::popcount((mask * magic) & 0xFF00000000000000ULL) >= 6;
        if (!has_enough_bits) {
            continue;
        }

        // iterate occupancies to check collision
        bool is_collision = true;
        bool is_used[1 << 12] = {false};
        Bitboard used_attacks[1 << 12] = {0};

        for (size_t i = 0; i < subset_cnt; ++i) {
            size_t idx = occupancies[i] * magic >> (64 - shift);

            if (!is_used[idx]) {
                is_used[idx] = true;
                used_attacks[idx] = attacks[i];

            } else if (used_attacks[idx] != attacks[i]) {
                is_collision = false;
                break;
            }
        }

        if (is_collision) {
            return magic;
        }
    }
}

std::array<Bitboard, SQUARE_CNT> magic_numbers(
    const std::array<Bitboard, SQUARE_CNT> &masks, const std::array<uint8_t, SQUARE_CNT> &shifts,
    const std::array<Direction, 4> &directions
) {
    std::array<Bitboard, SQUARE_CNT> result{};
    std::mt19937_64 rng(std::random_device{}());

    for (Square sq = SQUARE_A1; sq < SQUARE_CNT; ++sq) {
        result[sq] = find_magic(sq, masks[sq], shifts[sq], directions, rng);
    }

    return result;
}

} // namespace

std::array<Bitboard, SQUARE_CNT> rook_magics() {
    return magic_numbers(ROOK_MASKS, ROOK_SHIFTS, ROOK_DIRECTIONS);
}

std::array<Bitboard, SQUARE_CNT> bishop_magics() {
    return magic_numbers(BISHOP_MASKS, BISHOP_SHIFTS, BISHOP_DIRECTIONS);
}

} // namespace cheslib::detail
