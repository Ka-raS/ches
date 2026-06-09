#include <array>
#include <bit>
#include <iomanip>
#include <iostream>
#include <random>

namespace {

using File = uint8_t;
using Square = uint8_t;
using Bitboard = uint64_t;

constexpr Square SquareCNT = 64;

enum Direction : int8_t {
    North = 8,
    East = 1,
    South = -North,
    West = -East,

    NorthEast = North + East,
    SouthEast = South + East,
    SouthWest = -NorthEast,
    NorthWest = -SouthEast
};

bool has_square(const Bitboard board, const Square square) {
    return board & (1ull << square);
}

void set_square(Bitboard &board, const Square square) {
    board |= (1ull << square);
}

File file_of(const Square square) {
    return square & 7u;
}

Square next_square(const Square from, const int8_t step) {
    // check rank wraparound
    Square to = from + step;
    if (to >= SquareCNT) {
        return SquareCNT;
    }

    // check file wraparound
    const int d_file = file_of(from) - file_of(to);
    if (-2 <= d_file && d_file <= 2) {
        return to;
    } else {
        return SquareCNT;
    }
}

Bitboard sliding_attack_at(const Square from, const Bitboard occupancy, const std::array<Direction, 4> &directions) {
    Bitboard result = 0;

    for (const Direction dir : directions) {
        Square curr = next_square(from, dir);

        while (curr < SquareCNT) {
            set_square(result, curr);
            const bool is_blocked = has_square(occupancy, curr);
            if (is_blocked) {
                break;
            }
            curr = next_square(curr, dir);
        }
    }

    return result;
}

Bitboard sliding_blockers(const Square from, const std::array<Direction, 4> &directions) {
    Bitboard result = 0;

    for (const Direction dir : directions) {
        Square curr = next_square(from, dir);
        if (curr >= SquareCNT) {
            continue;
        }

        // exclude edge squares
        Square next = next_square(curr, dir);
        while (next < SquareCNT) {
            set_square(result, curr);
            curr = next;
            next = next_square(curr, dir);
        }
    }

    return result;
}

uint64_t find_magic(const Square from, const std::array<Direction, 4> &directions, std::mt19937_64 &rng) {
    constexpr size_t max_subsets = 1ull << 12;
    const Bitboard mask = sliding_blockers(from, directions);
    const size_t shift = std::popcount(mask);

    Bitboard attacks[max_subsets];
    Bitboard occupancies[max_subsets];

    { // find all occupancies and attacks
        size_t i = 0;
        Bitboard occupancy = 0;
        do {
            occupancies[i] = occupancy;
            attacks[i] = sliding_attack_at(from, occupancy, directions);
            occupancy = (occupancy - mask) & mask;
            ++i;
        } while (occupancy);
    }

    while (true) {
        const uint64_t magic = rng() & rng() & rng();

        const bool has_enough_bits = std::popcount((mask * magic) >> (64 - 8)) >= 6;
        if (!has_enough_bits) {
            continue;
        }

        // iterate occupancies to check collision
        bool is_collision = false;
        bool used_index[max_subsets] = {false};
        Bitboard used_attacks[max_subsets] = {0};

        for (size_t i = 0; i < (1ULL << shift); ++i) {
            size_t magic_index = (occupancies[i] * magic) >> (64 - shift);

            if (!used_index[magic_index]) {
                used_index[magic_index] = true;
                used_attacks[magic_index] = attacks[i];

            } else if (used_attacks[magic_index] != attacks[i]) {
                is_collision = true;
                break;
            }
        }

        if (!is_collision) {
            return magic;
        }
    }
}

std::array<uint64_t, SquareCNT> generate_magic_numbers(const std::array<Direction, 4> directions) {
    std::array<uint64_t, SquareCNT> result;
    std::mt19937_64 rng(37);

    for (Square sq = 0; sq < SquareCNT; ++sq) {
        result[sq] = find_magic(sq, directions, rng);
    }

    return result;
}

} // namespace

int main() {
    std::cout << "Rook magics:\n" << std::hex << std::setfill('0');
    for (uint64_t magic : generate_magic_numbers(std::array{North, East, South, West})) {
        std::cout << "0x" << std::setw(16) << magic << ",\n";
    }

    std::cout << "\nBishop magics:\n";
    for (uint64_t magic : generate_magic_numbers(std::array{NorthEast, SouthEast, SouthWest, NorthWest})) {
        std::cout << "0x" << std::setw(16) << magic << ",\n";
    }
}