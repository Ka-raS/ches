#include <array>
#include <random>

#include "attack_precompute.hpp"

namespace cheslib::detail {

namespace {

Bitboard find_magic(
    Square from, Bitboard mask, uint8_t shift, const std::array<Direction, 4> &directions, std::mt19937_64 &rng
) {
    constexpr size_t MAX_BLOCKERS = 12;
    constexpr size_t MAX_SUBSETS = 1U << MAX_BLOCKERS;

    // find all occupancies and attacks
    Bitboard attacks[MAX_SUBSETS] = {0};
    Bitboard occupancies[MAX_SUBSETS] = {0};
    const auto blocker_squares = blocker_positions(mask);
    const size_t subset_cnt = 1U << shift;

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
        bool is_collision = false;
        bool used_index[MAX_SUBSETS] = {false};
        Bitboard used_attacks[MAX_SUBSETS] = {0};

        for (size_t subset = 0; subset < subset_cnt; ++subset) {
            size_t index = magic_index(occupancies[subset], magic, shift);

            if (!used_index[index]) {
                used_index[index] = true;
                used_attacks[index] = attacks[subset];

            } else if (used_attacks[index] != attacks[subset]) {
                is_collision = true;
                break;
            }
        }

        if (!is_collision) {
            return magic;
        }
    }
}

} // namespace

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

} // namespace cheslib::detail
