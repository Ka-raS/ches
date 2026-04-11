#include <array>
#include <bit>
#include <fstream>
#include <iomanip>
#include <random>

#include "attack_tables.hpp"
#include "utils.hpp"

namespace {

using namespace ches;

constexpr size_t MaxBlockers = 12;
constexpr size_t MaxSubsets = 1U << MaxBlockers;
constexpr std::array<Direction, 4> RookDirections{Up, Right, Down, Left};
constexpr std::array<Direction, 4> BishopDirections{UpRight, DownRight, DownLeft, UpLeft};

Square shift_square(Square from, int8_t step) {
    // check rank wraparound
    Square to = Square(from + step);
    if (to >= SquareCNT) {
        return SquareCNT;
    }

    // check file wraparound
    File from_file = file_of(from);
    File to_file = file_of(to);
    if (std::abs(to_file - from_file) > 2) {
        return SquareCNT;
    }

    return to;
}

std::array<Square, MaxBlockers> blocker_positions(Bitboard mask) {
    std::array<Square, MaxBlockers> positions{};
    positions.fill(SquareCNT);
    size_t count = 0;

    for (Square sq = SquareA1; sq < SquareCNT; ++sq) {
        if (has_square(mask, sq)) {
            positions[count++] = sq;
        }
    }

    return positions;
}

Bitboard occupancy_from_subset(
    size_t subset, size_t blocker_cnt, const std::array<Square, MaxBlockers> &blocker_pos
) {
    Bitboard occupancy = 0;

    for (size_t i = 0; i < blocker_cnt; ++i) {
        if (blocker_pos[i] >= SquareCNT) {
            continue;
        }

        bool is_in_subset = subset & (1U << i);
        if (is_in_subset) {
            set_square(occupancy, blocker_pos[i]);
        }
    }

    return occupancy;
}

Bitboard sliding_attack_at(
    Square from, Bitboard occupancy, const std::array<Direction, 4> &directions
) {
    Bitboard result = 0;

    for (Direction dir : directions) {
        Square curr = shift_square(from, dir);

        while (curr < SquareCNT) {
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

Bitboard sliding_blockers(Square from, const std::array<Direction, 4> &directions) {
    Bitboard result = 0;

    for (Direction dir : directions) {
        Square curr = shift_square(from, dir);
        if (curr >= SquareCNT) {
            continue;
        }

        // exclude edge squares
        Square next = shift_square(curr, dir);
        while (next < SquareCNT) {
            set_square(result, curr);
            curr = next;
            next = shift_square(curr, dir);
        }
    }

    return result;
}

uint64_t find_magic(
    Square from, Bitboard mask, uint8_t shift, const std::array<Direction, 4> &directions,
    std::mt19937_64 &rng
) {

    // find all occupancies and attacks
    Bitboard attacks[MaxSubsets] = {0};
    Bitboard occupancies[MaxSubsets] = {0};
    const auto blocker_squares = blocker_positions(mask);
    const size_t subset_cnt = 1U << shift;

    for (size_t subset = 0; subset < subset_cnt; ++subset) {
        occupancies[subset] = occupancy_from_subset(subset, shift, blocker_squares);
        attacks[subset] = sliding_attack_at(from, occupancies[subset], directions);
    }

    detail::MagicInfo info{mask, 0, 0, shift};

    while (true) {
        const uint64_t magic = info.magic = rng() & rng() & rng();

        bool has_enough_bits = std::popcount((mask * magic) >> (64 - 8)) >= 6;
        if (!has_enough_bits) {
            continue;
        }

        // iterate occupancies to check collision
        bool is_collision = false;
        bool used_index[MaxSubsets] = {false};
        Bitboard used_attacks[MaxSubsets] = {0};

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

std::array<uint64_t, SquareCNT> generate_magic_numbers(const std::array<Direction, 4> &directions) {
    std::array<uint64_t, SquareCNT> result{};
    std::mt19937_64 rng(37);

    for (Square sq = SquareA1; sq < SquareCNT; ++sq) {
        Bitboard mask = sliding_blockers(sq, directions);
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
        out << "0x" << std::setw(16) << magic << ",\n";
    }

    out << '\n';

    for (uint64_t magic : generate_magic_numbers(BishopDirections)) {
        out << "0x" << std::setw(16) << magic << ",\n";
    }

    out.close();
}