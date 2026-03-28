#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <cstdlib>

#include "cheslib/constants.hpp"

namespace cheslib {

constexpr uint64_t knight_attacks(int sq);
constexpr uint64_t king_attacks(int sq);
constexpr uint64_t pawn_attacks(int sq, bool is_white);
constexpr uint64_t rook_attacks(int sq, uint64_t occupancy);
constexpr uint64_t bishop_attacks(int sq, uint64_t occupancy);
constexpr uint64_t queen_attacks(int sq, uint64_t occupancy);

namespace {

constexpr bool is_inbounds(int x, int y) {
    return 0 <= x && x < BOARD_SIZE && 0 <= y && y < BOARD_SIZE;
}

template <size_t N> constexpr std::array<uint64_t, NUM_SQUARES> stepping_attacks(const std::array<int, N> &offsets) {
    std::array<uint64_t, NUM_SQUARES> result = {0};

    for (int sq = 0; sq < NUM_SQUARES; ++sq) {
        int x = sq % BOARD_SIZE;
        int y = sq / BOARD_SIZE;

        for (int off : offsets) {
            int dx = off % BOARD_SIZE;
            int dy = off / BOARD_SIZE;
            int nx = x + dx;
            int ny = y + dy;

            if (is_inbounds(nx, ny)) {
                int next = ny * BOARD_SIZE + nx;
                result[sq] |= 1ULL << next;
            }
        }
    }

    return result;
}

template <size_t N> constexpr std::array<uint64_t, NUM_SQUARES> sliding_blockers(const std::array<int, N> &directions) {
    std::array<uint64_t, NUM_SQUARES> result = {0};

    for (int sq = 0; sq < NUM_SQUARES; ++sq) {
        int x = sq % BOARD_SIZE;
        int y = sq / BOARD_SIZE;

        for (int dir : directions) {
            int dx = dir % BOARD_SIZE;
            int dy = dir / BOARD_SIZE;

            for (int nx = x + dx, ny = y + dy; is_inbounds(nx + dx, ny + dy); nx += dx, ny += dy) {
                int next = ny * BOARD_SIZE + nx;
                result[sq] |= 1ULL << next;
            }
        }
    }

    return result;
}

template <size_t N>
constexpr uint64_t sliding_attack_at(int sq, uint64_t blockers, const std::array<int, N> &directions) {
    uint64_t result = 0;
    int x = sq % BOARD_SIZE;
    int y = sq / BOARD_SIZE;

    for (int dir : directions) {
        int dx = dir % BOARD_SIZE;
        int dy = dir / BOARD_SIZE;

        for (int nx = x + dx, ny = y + dy; is_inbounds(nx, ny); nx += dx, ny += dy) {
            int next = ny * BOARD_SIZE + nx;
            result |= 1ULL << next;

            bool is_blocked = blockers & (1ULL << next);
            if (is_blocked) {
                break;
            }
        }
    }

    return result;
}

template <size_t ATTACKS_N, size_t DIRS_N>
constexpr std::array<uint64_t, ATTACKS_N> sliding_attacks(
    const std::array<uint64_t, NUM_SQUARES> &masks, const std::array<int, NUM_SQUARES> &shifts,
    const std::array<size_t, NUM_SQUARES> &offsets, const std::array<int, DIRS_N> &directions
) {
    std::array<uint64_t, ATTACKS_N> attacks = {0};

    for (int sq = 0; sq < NUM_SQUARES; ++sq) {
        const uint64_t all_blockers = masks[sq];
        const int num_blockers = shifts[sq];

        // map all_blockers bitboard to position array
        int blocker_positions[num_blockers];

        for (int i = 0, pos_count = 0; i < NUM_SQUARES; ++i) {
            bool has_blocker = masks[sq] & (1ULL << i);
            if (has_blocker) {
                blocker_positions[pos_count++] = i;
            }
        }

        // generate all blockers subsets
        for (uint32_t subset = 0; subset < (1U << num_blockers); ++subset) {
            // build blockers bitboard
            uint64_t blockers = 0;

            for (int i = 0; i < num_blockers; ++i) {
                bool is_in_subset = subset & (1U << i);
                if (is_in_subset) {
                    blockers |= 1ULL << blocker_positions[i];
                }
            }

            attacks[offsets[sq] + subset] = sliding_attack_at(sq, blockers, directions);
        }
    }

    return attacks;
}

constexpr std::array<int, NUM_SQUARES> popcounts(const std::array<uint64_t, NUM_SQUARES> &masks) {
    std::array<int, NUM_SQUARES> result = {0};

    for (int sq = 0; sq < NUM_SQUARES; ++sq) {
        result[sq] = std::popcount(masks[sq]);
    }

    return result;
}

constexpr std::array<size_t, NUM_SQUARES> occupancy_offsets(const std::array<int, NUM_SQUARES> &shifts) {
    std::array<size_t, NUM_SQUARES> result = {0};

    for (int sq = 1; sq < NUM_SQUARES; ++sq) {
        size_t prev_size = 1ULL << shifts[sq - 1];
        result[sq] = result[sq - 1] + prev_size;
    }

    return result;
}

constexpr std::array<int, 8> KNIGHT_DIRS = {UP_RIGHT + UP,    UP_RIGHT + RIGHT, DOWN_RIGHT + RIGHT, DOWN_RIGHT + DOWN,
                                            DOWN_LEFT + DOWN, DOWN_LEFT + LEFT, UP_LEFT + LEFT,     UP_LEFT + UP};
constexpr std::array<int, 8> KING_DIRS = {UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT};
constexpr std::array<int, 4> ROOK_DIRS = {UP, RIGHT, DOWN, LEFT};
constexpr std::array<int, 4> BISHOP_DIRS = {UP_RIGHT, DOWN_RIGHT, DOWN_LEFT, UP_LEFT};
constexpr std::array<int, 2> WHITE_PAWN_DIRS = {UP_LEFT, UP_RIGHT};
constexpr std::array<int, 2> BLACK_PAWN_DIRS = {DOWN_LEFT, DOWN_RIGHT};

constexpr std::array<uint64_t, NUM_SQUARES> KNIGHT_ATTACKS = stepping_attacks(KNIGHT_DIRS);
constexpr std::array<uint64_t, NUM_SQUARES> KING_ATTACKS = stepping_attacks(KING_DIRS);
constexpr std::array<uint64_t, NUM_SQUARES> WHITE_PAWN_ATTACKS = stepping_attacks(WHITE_PAWN_DIRS);
constexpr std::array<uint64_t, NUM_SQUARES> BLACK_PAWN_ATTACKS = stepping_attacks(BLACK_PAWN_DIRS);

constexpr std::array<uint64_t, NUM_SQUARES> ROOK_MASKS = sliding_blockers(ROOK_DIRS);
constexpr std::array<uint64_t, NUM_SQUARES> BISHOP_MASKS = sliding_blockers(BISHOP_DIRS);
constexpr std::array<int, NUM_SQUARES> ROOK_SHIFTS = popcounts(ROOK_MASKS);
constexpr std::array<int, NUM_SQUARES> BISHOP_SHIFTS = popcounts(BISHOP_MASKS);
constexpr std::array<size_t, NUM_SQUARES> ROOK_OFFSETS = occupancy_offsets(ROOK_SHIFTS);
constexpr std::array<size_t, NUM_SQUARES> BISHOP_OFFSETS = occupancy_offsets(BISHOP_SHIFTS);

// based on last combination:               index begin         + size
constexpr size_t ROOK_ATTACKS_SIZE = ROOK_OFFSETS.back() + (1ULL << ROOK_SHIFTS.back());
constexpr size_t BISHOP_ATTACKS_SIZE = BISHOP_OFFSETS.back() + (1ULL << BISHOP_SHIFTS.back());

constexpr std::array<uint64_t, ROOK_ATTACKS_SIZE> ROOK_ATTACKS =
    sliding_attacks<ROOK_ATTACKS_SIZE>(ROOK_MASKS, ROOK_SHIFTS, ROOK_OFFSETS, ROOK_DIRS);

constexpr std::array<uint64_t, BISHOP_ATTACKS_SIZE> BISHOP_ATTACKS =
    sliding_attacks<BISHOP_ATTACKS_SIZE>(BISHOP_MASKS, BISHOP_SHIFTS, BISHOP_OFFSETS, BISHOP_DIRS);

} // namespace

constexpr uint64_t knight_attacks(int sq) {
    return KNIGHT_ATTACKS[sq];
}

constexpr uint64_t king_attacks(int sq) {
    return KING_ATTACKS[sq];
}

constexpr uint64_t pawn_attacks(int sq, bool is_white) {
    return is_white ? WHITE_PAWN_ATTACKS[sq] : BLACK_PAWN_ATTACKS[sq];
}

constexpr uint64_t rook_attacks(int sq, uint64_t occupancy) {
    // uint64_t attack = attacks[offsets[sq] + (all_pieces & masks[sq]) * magic[sq] >> shift[sq]];
    return ROOK_ATTACKS[ROOK_OFFSETS[sq] + (occupancy & ROOK_MASKS[sq]) >> ROOK_SHIFTS[sq]];
}

constexpr uint64_t bishop_attacks(int sq, uint64_t occupancy) {
    return BISHOP_ATTACKS[BISHOP_OFFSETS[sq] + (occupancy & BISHOP_MASKS[sq]) >> BISHOP_SHIFTS[sq]];
}

constexpr uint64_t queen_attacks(int sq, uint64_t occupancy) {
    return rook_attacks(sq, occupancy) | bishop_attacks(sq, occupancy);
}

} // namespace cheslib
