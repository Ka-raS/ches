#include <array>
#include <bit>
#include <fstream>
#include <iomanip>
#include <random>

#include "attack_tables.hpp"
#include "utils.hpp"

namespace {

constexpr size_t MaxBlockers = 12;
constexpr size_t MaxSubsets = 1U << MaxBlockers;
constexpr std::array<ches::Direction, 4> RookDirections{ches::Up, ches::Right, ches::Down, ches::Left};
constexpr std::array<ches::Direction, 4> BishopDirections{ches::UpRight, ches::DownRight, ches::DownLeft, ches::UpLeft};

ches::Square shift_square(ches::Square from, int8_t step) {
    // check rank wraparound
    ches::Square to = ches::Square(from + step);
    if (to >= ches::SquareCNT) {
        return ches::SquareCNT;
    }

    // check file wraparound
    ches::File from_file = ches::file_of(from);
    ches::File to_file = ches::file_of(to);
    if (std::abs(to_file - from_file) > 2) {
        return ches::SquareCNT;
    }

    return to;
}

std::array<ches::Square, MaxBlockers> blocker_positions(ches::Bitboard mask) {
    std::array<ches::Square, MaxBlockers> positions{};
    positions.fill(ches::SquareCNT);
    size_t count = 0;

    for (ches::Square sq = ches::SquareA1; sq < ches::SquareCNT; ++sq) {
        if (ches::has_square(mask, sq)) {
            positions[count++] = sq;
        }
    }

    return positions;
}

ches::Bitboard occupancy_from_subset(
    size_t subset, size_t blocker_cnt, const std::array<ches::Square, MaxBlockers> &blocker_pos
) {
    ches::Bitboard occupancy = 0;

    for (size_t i = 0; i < blocker_cnt; ++i) {
        if (blocker_pos[i] >= ches::SquareCNT) {
            continue;
        }

        bool is_in_subset = subset & (1U << i);
        if (is_in_subset) {
            ches::set_square(occupancy, blocker_pos[i]);
        }
    }

    return occupancy;
}

ches::Bitboard sliding_attack_at(
    ches::Square from, ches::Bitboard occupancy, const std::array<ches::Direction, 4> &directions
) {
    ches::Bitboard result = 0;

    for (ches::Direction dir : directions) {
        ches::Square curr = shift_square(from, dir);

        while (curr < ches::SquareCNT) {
            ches::set_square(result, curr);
            bool is_blocked = ches::has_square(occupancy, curr);
            if (is_blocked) {
                break;
            }
            curr = shift_square(curr, dir);
        }
    }

    return result;
}

ches::Bitboard sliding_blockers(ches::Square from, const std::array<ches::Direction, 4> &directions) {
    ches::Bitboard result = 0;

    for (ches::Direction dir : directions) {
        ches::Square curr = shift_square(from, dir);
        if (curr >= ches::SquareCNT) {
            continue;
        }

        // exclude edge squares
        ches::Square next = shift_square(curr, dir);
        while (next < ches::SquareCNT) {
            ches::set_square(result, curr);
            curr = next;
            next = shift_square(curr, dir);
        }
    }

    return result;
}

uint64_t find_magic(
    ches::Square from, ches::Bitboard mask, uint8_t shift, const std::array<ches::Direction, 4> &directions,
    std::mt19937_64 &rng
) {

    // find all occupancies and attacks
    ches::Bitboard attacks[MaxSubsets] = {0};
    ches::Bitboard occupancies[MaxSubsets] = {0};
    const auto blocker_squares = blocker_positions(mask);
    const size_t subset_cnt = 1U << shift;

    for (size_t subset = 0; subset < subset_cnt; ++subset) {
        occupancies[subset] = occupancy_from_subset(subset, shift, blocker_squares);
        attacks[subset] = sliding_attack_at(from, occupancies[subset], directions);
    }

    ches::detail::MagicInfo info{mask, 0, 0, shift};

    while (true) {
        const uint64_t magic = info.magic = rng() & rng() & rng();

        bool has_enough_bits = std::popcount((mask * magic) >> (64 - 8)) >= 6;
        if (!has_enough_bits) {
            continue;
        }

        // iterate occupancies to check collision
        bool is_collision = false;
        bool used_index[MaxSubsets] = {false};
        ches::Bitboard used_attacks[MaxSubsets] = {0};

        for (size_t subset = 0; subset < subset_cnt; ++subset) {
            size_t index = info.index(occupancies[subset]);

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

std::array<uint64_t, ches::SquareCNT> generate_magic_numbers(const std::array<ches::Direction, 4> &directions) {
    std::array<uint64_t, ches::SquareCNT> result{};
    std::mt19937_64 rng(37);

    for (ches::Square sq = ches::SquareA1; sq < ches::SquareCNT; ++sq) {
        ches::Bitboard mask = sliding_blockers(sq, directions);
        uint32_t shift = std::popcount(mask);
        result[sq] = find_magic(sq, mask, shift, directions, rng);
    }

    return result;
}

} // namespace

int main() {
    std::ofstream out("magics.txt");
    out << std::hex << std::setfill('0');

    for (uint64_t magic : generate_magic_numbers(RookDirections)) {
        out << "0x" << std::setw(16) << magic << "ULL,\n";
    }

    out << '\n';

    for (uint64_t magic : generate_magic_numbers(BishopDirections)) {
        out << "0x" << std::setw(16) << magic << "ULL,\n";
    }

    out.close();
}