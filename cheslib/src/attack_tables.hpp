#pragma once

#include <array>
#include <bit>
#include <cassert>
#include <cstdint>
#include <cstdlib>

#include "cheslib/types.hpp"

namespace cheslib {

constexpr Bitboard knight_attacks(Square from);
constexpr Bitboard king_attacks(Square from);
constexpr Bitboard white_pawn_attacks(Square from);
constexpr Bitboard black_pawn_attacks(Square from);
constexpr Bitboard rook_attacks(Square from, Bitboard occupancy);
constexpr Bitboard bishop_attacks(Square from, Bitboard occupancy);

namespace detail {

// generate new rook magic numbers
std::array<Bitboard, SQUARE_CNT> rook_magics();

// generate new bishop magic numbers
std::array<Bitboard, SQUARE_CNT> bishop_magics();

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

constexpr Square add_square(Square from, int8_t step) {
    Square to = Square(from + step);
    if (to >= SQUARE_CNT) {
        return SQUARE_CNT;
    }

    // check file wraparound
    File from_file = File(from % (uint8_t)FILE_CNT);
    File to_file = File(to % (uint8_t)FILE_CNT);
    if (std::abs(to_file - from_file) > 2) {
        return SQUARE_CNT;
    }

    return to;
}

template <size_t N> constexpr std::array<Bitboard, SQUARE_CNT> stepping_attacks(const std::array<int8_t, N> &steps) {
    std::array<Bitboard, SQUARE_CNT> result = {0};

    for (Square sq = SQUARE_A1; sq < SQUARE_CNT; ++sq) {
        for (int8_t step : steps) {
            Square next = add_square(sq, step);
            if (next < SQUARE_CNT) {
                result[sq] |= 1ULL << next;
            }
        }
    }

    return result;
}

constexpr std::array<Bitboard, SQUARE_CNT> sliding_blockers(const std::array<Direction, 4> &directions) {
    std::array<Bitboard, SQUARE_CNT> result = {0};

    for (Square sq = SQUARE_A1; sq < SQUARE_CNT; ++sq) {
        for (Direction dir : directions) {
            Square curr = add_square(sq, dir);
            if (curr >= SQUARE_CNT) {
                continue;
            }

            // exclude edge squares
            Square next = add_square(curr, dir);
            while (next < SQUARE_CNT) {
                result[sq] |= 1ULL << curr;
                curr = next;
                next = add_square(curr, dir);
            }
        }
    }

    return result;
}

constexpr std::array<Square, 12> blockers_from_mask(Bitboard mask) {
    std::array<Square, 12> blocker_squares{};
    blocker_squares.fill(SQUARE_CNT);
    size_t count = 0;

    for (Square s = SQUARE_A1; s < SQUARE_CNT; ++s) {
        bool has_blocker = mask & (1ULL << s);
        if (has_blocker) {
            blocker_squares[count++] = s;
        }
    }

    return blocker_squares;
}

constexpr Bitboard occupancy_from_subset(
    uint32_t subset, size_t blocker_cnt, const std::array<Square, 12> &blocker_squares
) {
    Bitboard occupancy = 0;
    for (size_t i = 0; i < blocker_cnt; ++i) {
        bool in_subset = subset & (1U << i);
        if (in_subset && blocker_squares[i] < SQUARE_CNT) {
            occupancy |= 1ULL << blocker_squares[i];
        }
    }
    return occupancy;
}

template <size_t N>
constexpr Bitboard sliding_attack_at(Square from, Bitboard occupancy, const std::array<Direction, N> &directions) {
    Bitboard result = 0;

    for (Direction dir : directions) {
        Square curr = add_square(from, dir);

        while (curr < SQUARE_CNT) {
            result |= 1ULL << curr;
            bool is_blocked = occupancy & (1ULL << curr);
            if (is_blocked) {
                break;
            }
            curr = add_square(curr, dir);
        }
    }

    return result;
}

template <size_t N>
constexpr std::array<Bitboard, N> sliding_attacks(
    const std::array<Bitboard, SQUARE_CNT> &masks, const std::array<Bitboard, SQUARE_CNT> &magics,
    const std::array<uint8_t, SQUARE_CNT> &shifts, const std::array<uint32_t, SQUARE_CNT> &offsets,
    const std::array<Direction, 4> &directions
) {
    std::array<Bitboard, N> attacks = {0};

    for (Square sq = SQUARE_A1; sq < SQUARE_CNT; ++sq) {
        std::array<Square, 12> blocker_squares = blockers_from_mask(masks[sq]);
        const size_t blocker_cnt = shifts[sq];
        const size_t subset_cnt = 1U << blocker_cnt;

        // iterate all occupancy subsets
        for (size_t subset = 0; subset < subset_cnt; ++subset) {
            Bitboard occupancy = occupancy_from_subset(subset, blocker_cnt, blocker_squares);

            size_t index = occupancy * magics[sq] >> (64 - shifts[sq]);
            attacks[offsets[sq] + index] = sliding_attack_at(sq, occupancy, directions);
        }
    }

    return attacks;
}

inline constexpr std::array<int8_t, 8> KNIGHT_STEPS = {UP_RIGHT + UP,     UP_RIGHT + RIGHT, DOWN_RIGHT + RIGHT,
                                                       DOWN_RIGHT + DOWN, DOWN_LEFT + DOWN, DOWN_LEFT + LEFT,
                                                       UP_LEFT + LEFT,    UP_LEFT + UP};
inline constexpr std::array<int8_t, 8> KING_STEPS = {UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT};
inline constexpr std::array<int8_t, 2> WHITE_PAWN_STEPS = {UP_LEFT, UP_RIGHT};
inline constexpr std::array<int8_t, 2> BLACK_PAWN_STEPS = {DOWN_LEFT, DOWN_RIGHT};
inline constexpr std::array<Direction, 4> ROOK_DIRECTIONS = {UP, RIGHT, DOWN, LEFT};
inline constexpr std::array<Direction, 4> BISHOP_DIRECTIONS = {UP_RIGHT, DOWN_RIGHT, DOWN_LEFT, UP_LEFT};

inline constexpr std::array<Bitboard, SQUARE_CNT> KNIGHT_ATTACKS = stepping_attacks(KNIGHT_STEPS);
inline constexpr std::array<Bitboard, SQUARE_CNT> KING_ATTACKS = stepping_attacks(KING_STEPS);
inline constexpr std::array<Bitboard, SQUARE_CNT> WHITE_PAWN_ATTACKS = stepping_attacks(WHITE_PAWN_STEPS);
inline constexpr std::array<Bitboard, SQUARE_CNT> BLACK_PAWN_ATTACKS = stepping_attacks(BLACK_PAWN_STEPS);

inline constexpr std::array<Bitboard, SQUARE_CNT> ROOK_MASKS = sliding_blockers(ROOK_DIRECTIONS);
inline constexpr std::array<Bitboard, SQUARE_CNT> BISHOP_MASKS = sliding_blockers(BISHOP_DIRECTIONS);
inline constexpr std::array<uint8_t, SQUARE_CNT> ROOK_SHIFTS = popcounts(ROOK_MASKS);
inline constexpr std::array<uint8_t, SQUARE_CNT> BISHOP_SHIFTS = popcounts(BISHOP_MASKS);
inline constexpr std::array<uint32_t, SQUARE_CNT> ROOK_OFFSETS = occupancy_offsets(ROOK_SHIFTS);
inline constexpr std::array<uint32_t, SQUARE_CNT> BISHOP_OFFSETS = occupancy_offsets(BISHOP_SHIFTS);

// generated on 2026-04-01
inline constexpr std::array<Bitboard, SQUARE_CNT> ROOK_MAGICS = {
    0x0080002852814000ULL, 0x0040004020001000ULL, 0x0100100840200100ULL, 0x0100100114886100ULL, 0x0e00304820220004ULL,
    0x1200040200100108ULL, 0x4500089406004500ULL, 0x020002408404b201ULL, 0x0402002200884100ULL, 0x8000400040201009ULL,
    0x0008802000801008ULL, 0x01020010400a0420ULL, 0x3382808004000800ULL, 0x0022001002000904ULL, 0x8104000884010230ULL,
    0xa020800b49002180ULL, 0x0011020020804200ULL, 0x1040004020100040ULL, 0x0020004010080040ULL, 0x1008008010000884ULL,
    0x1101850008001100ULL, 0x0002010100040008ULL, 0x0200808001000200ULL, 0x1000ca0000831444ULL, 0x0200408200210200ULL,
    0x4810004440042000ULL, 0x2c00110100200840ULL, 0x0000100080080080ULL, 0x4000040080080080ULL, 0x0100020080800400ULL,
    0x0402000200484144ULL, 0x106041060000804cULL, 0x1100400020800081ULL, 0xc300201000400041ULL, 0x8100100181802004ULL,
    0x7488801000800800ULL, 0x8060080080800400ULL, 0x0002000402000810ULL, 0x2020800200800100ULL, 0x008600b402000041ULL,
    0x8480002000404004ULL, 0x0020008040028030ULL, 0x0090120020820040ULL, 0x0002001044220008ULL, 0x3008008004008008ULL,
    0x2012000410020008ULL, 0x0a00100208040001ULL, 0xc00100805402000dULL, 0x0224402480010700ULL, 0x0480200085c00480ULL,
    0x4200190040200100ULL, 0x6000801000080080ULL, 0x0a00080004008080ULL, 0x1202000280040080ULL, 0x0410024801100400ULL,
    0x4020208421004200ULL, 0x0005510a20800043ULL, 0x00004000e0198301ULL, 0x01000b0120001041ULL, 0x1000900005002089ULL,
    0x2509000208001085ULL, 0x0002001001880402ULL, 0x0400081029009204ULL, 0x0000104404228502ULL
};
inline constexpr std::array<Bitboard, SQUARE_CNT> BISHOP_MAGICS = {
    0x0041221096008100ULL, 0x2002848802084000ULL, 0x0004040404408000ULL, 0x1404404288000690ULL, 0x0404042200111000ULL,
    0x3050882049010010ULL, 0x0804010829041080ULL, 0x4028260050081808ULL, 0x0800100222480210ULL, 0x0000210404004048ULL,
    0x90410800840a9001ULL, 0x04100440488412a0ULL, 0x8000508820048001ULL, 0x0444008210400000ULL, 0x0000090401204844ULL,
    0x0001308a08010444ULL, 0x8011080810010802ULL, 0x00140c08100cb200ULL, 0x0010800808004209ULL, 0x0c08004104130022ULL,
    0x2202000400a21000ULL, 0x0011a04601900805ULL, 0x0804200084040200ULL, 0x0000200084231803ULL, 0x0208545020cc9000ULL,
    0x0004201010024080ULL, 0x0681044010040220ULL, 0x0c2008000a081050ULL, 0x0004082014002000ULL, 0x0010088001080100ULL,
    0x4322004008843008ULL, 0x0082020414410080ULL, 0x0808041200842008ULL, 0x0000824800200800ULL, 0x2056028200101020ULL,
    0x0300020080a80080ULL, 0x0020009004430040ULL, 0x0001190104220044ULL, 0x2041062088042400ULL, 0x0002220020004420ULL,
    0x00122820f4000841ULL, 0x3044014802880820ULL, 0x4008101098001002ULL, 0x3000220102402400ULL, 0xa20008a100410400ULL,
    0xa044008882000501ULL, 0x0010105105048442ULL, 0x80300481809ea100ULL, 0x010080d008210082ULL, 0x0820808090102001ULL,
    0x1400010280d00012ULL, 0x020000020a0a0080ULL, 0x0000001102021140ULL, 0x0200202202021002ULL, 0x0e04501001012101ULL,
    0x002018011a608200ULL, 0x0082240414010821ULL, 0x2280004444101801ULL, 0x0000000029180822ULL, 0x3c0090481820a804ULL,
    0x1420001004850400ULL, 0x1088220408108101ULL, 0x400088202800a10aULL, 0x1404080820448200ULL
};

// based on last occupancy:  index begin         + size
inline constexpr size_t ROOK_SIZE = ROOK_OFFSETS.back() + (1ULL << ROOK_SHIFTS.back());
inline constexpr size_t BISHOP_SIZE = BISHOP_OFFSETS.back() + (1ULL << BISHOP_SHIFTS.back());

inline constexpr std::array<Bitboard, ROOK_SIZE> ROOK_ATTACKS =
    sliding_attacks<ROOK_SIZE>(ROOK_MASKS, ROOK_MAGICS, ROOK_SHIFTS, ROOK_OFFSETS, ROOK_DIRECTIONS);
inline constexpr std::array<Bitboard, BISHOP_SIZE> BISHOP_ATTACKS =
    sliding_attacks<BISHOP_SIZE>(BISHOP_MASKS, BISHOP_MAGICS, BISHOP_SHIFTS, BISHOP_OFFSETS, BISHOP_DIRECTIONS);

} // namespace detail

constexpr Bitboard knight_attacks(Square from) {
    assert(from < SQUARE_CNT);
    return detail::KNIGHT_ATTACKS[from];
}

constexpr Bitboard king_attacks(Square from) {
    assert(from < SQUARE_CNT);
    return detail::KING_ATTACKS[from];
}

constexpr Bitboard white_pawn_attacks(Square from) {
    assert(from < SQUARE_CNT);
    return detail::WHITE_PAWN_ATTACKS[from];
}

constexpr Bitboard black_pawn_attacks(Square from) {
    assert(from < SQUARE_CNT);
    return detail::BLACK_PAWN_ATTACKS[from];
}

constexpr Bitboard rook_attacks(Square from, Bitboard occupancy) {
    assert(from < SQUARE_CNT);

    Bitboard mask = detail::ROOK_MASKS[from];
    Bitboard magic = detail::ROOK_MAGICS[from];
    uint8_t shift = detail::ROOK_SHIFTS[from];
    uint32_t offset = detail::ROOK_OFFSETS[from];

    size_t index = (occupancy & mask) * magic >> (64 - shift);
    return detail::ROOK_ATTACKS[offset + index];
}

constexpr Bitboard bishop_attacks(Square from, Bitboard occupancy) {
    assert(from < SQUARE_CNT);

    Bitboard mask = detail::BISHOP_MASKS[from];
    Bitboard magic = detail::BISHOP_MAGICS[from];
    uint8_t shift = detail::BISHOP_SHIFTS[from];
    uint32_t offset = detail::BISHOP_OFFSETS[from];

    size_t index = (occupancy & mask) * magic >> (64 - shift);
    return detail::BISHOP_ATTACKS[offset + index];
}

} // namespace cheslib
