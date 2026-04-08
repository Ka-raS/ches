#include <array>
#include <span>

#include "attack_tables.hpp"
#include "utils.hpp"

namespace ches::detail {

namespace {

consteval Square shift_square(Square from, int8_t step) {
    assert(from < SquareCNT);

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

consteval std::array<Bitboard, SquareCNT> stepping_attacks(std::span<const int8_t> steps) {
    std::array<Bitboard, SquareCNT> result = {0};

    for (Square sq = SquareA1; sq < SquareCNT; ++sq) {
        for (int8_t step : steps) {
            Square next = shift_square(sq, step);
            if (next < SquareCNT) {
                set_square(result[sq], next);
            }
        }
    }

    return result;
}

consteval Bitboard sliding_blockers(Square from, const std::array<Direction, 4> &directions) {
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

consteval Bitboard sliding_attack_at(Square from, Bitboard occupancy, const std::array<Direction, 4> &directions) {
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

constexpr size_t MaxBlockers = 12; // of rook
constexpr size_t MaxSubsets = 1U << MaxBlockers;

consteval std::array<Square, MaxBlockers> blocker_positions(Bitboard mask) {
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

consteval Bitboard occupancy_from_subset(
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

template <size_t N>
consteval std::array<Bitboard, N> sliding_attacks(
    const std::array<MagicInfo, SquareCNT> &magics, const std::array<Direction, 4> &directions
) {
    std::array<Bitboard, N> attacks = {0};

    for (Square sq = SquareA1; sq < SquareCNT; ++sq) {
        const auto &[mask, magic, offset, shift] = magics[sq];

        const size_t blocker_cnt = shift;
        const size_t subset_cnt = 1U << blocker_cnt;
        const auto blocker_pos = blocker_positions(mask);

        // iterate all occupancy subsets
        for (size_t subset = 0; subset < subset_cnt; ++subset) {
            Bitboard occupancy = occupancy_from_subset(subset, blocker_cnt, blocker_pos);
            size_t index = magics[sq].index(occupancy);

            attacks[offset + index] = sliding_attack_at(sq, occupancy, directions);
        }
    }

    return attacks;
}

consteval std::array<MagicInfo, SquareCNT> magic_infos(
    const std::array<uint64_t, SquareCNT> &magic_numbers, const std::array<Direction, 4> &directions
) {
    std::array<MagicInfo, SquareCNT> result;

    for (Square sq = SquareA1; sq < SquareCNT; ++sq) {
        uint32_t offset;
        if (sq == SquareA1) {
            offset = 0;
        } else {
            const uint32_t prev_table_size = 1U << result[sq - 1].shift;
            offset = result[sq - 1].offset + prev_table_size;
        }

        uint64_t magic = magic_numbers[sq];
        Bitboard mask = sliding_blockers(sq, directions);
        uint8_t shift = std::popcount(mask);

        result[sq] = {mask, magic, offset, shift};
    }

    return result;
}

// generated on 2026-04-06 using tools/find_magics.cpp
constexpr std::array<uint64_t, SquareCNT> RookMagicNumbers = {
    0x0080008040002010ULL, 0x22c0042000100040ULL, 0x8080100020008008ULL, 0xc100100100080421ULL, 0x4080020800810400ULL,
    0x2100040001000802ULL, 0x4400020110b01804ULL, 0x0200088200412c03ULL, 0x2016800120400288ULL, 0x1108808020004000ULL,
    0x0240802000100080ULL, 0x9281000900100020ULL, 0x0006001200042008ULL, 0x0042000200081004ULL, 0x1046000811020004ULL,
    0x2014801041000080ULL, 0x0020308000400080ULL, 0x000d020044802204ULL, 0x0840808010002004ULL, 0x0010004008040040ULL,
    0x0802020008218410ULL, 0x0c41010002080400ULL, 0x0000040041500208ULL, 0x4000020000806401ULL, 0x0480004240016002ULL,
    0x2001200280400080ULL, 0x0100110100402000ULL, 0x4080080080100084ULL, 0x0044000480800800ULL, 0x1142000404002010ULL,
    0x0001000100020004ULL, 0x8a1000460004008dULL, 0x02c0085080800020ULL, 0x4010804008802000ULL, 0x0300802000801000ULL,
    0x2000100282800800ULL, 0x1000800400800800ULL, 0x080c800200800400ULL, 0x1086002412000829ULL, 0x0000004082002104ULL,
    0x7028882040008000ULL, 0x0000810022020040ULL, 0x0020020400101000ULL, 0x10120040a1120008ULL, 0x2808002040040400ULL,
    0x0202011058220004ULL, 0x1009000a00150004ULL, 0x804c8680cc220001ULL, 0x0802004284350200ULL, 0x2200802100400100ULL,
    0x00200080a0100180ULL, 0x1010002208110100ULL, 0x000a800400080080ULL, 0x111a008004000280ULL, 0x8000a12802104400ULL,
    0x0100040840a10200ULL, 0xa442004821001082ULL, 0x224200e101104282ULL, 0x9009004008102001ULL, 0x0000100005002109ULL,
    0x1012002008100402ULL, 0x0005004400288201ULL, 0x0120080630088504ULL, 0x0000088c00204102ULL
};

// the numbers mason what do they mean????
constexpr std::array<uint64_t, SquareCNT> BishopMagicNumbers = {
    0x202024a480810200ULL, 0x8104040800410401ULL, 0x8004194202004102ULL, 0x9008204040000200ULL, 0x0308484004403400ULL,
    0x0402180208040440ULL, 0x0311011010840200ULL, 0x0040184804100800ULL, 0x0010400208420c81ULL, 0x0120101091204080ULL,
    0xa08090140940d880ULL, 0x2100844401801093ULL, 0x0002040420100080ULL, 0x220002080208c000ULL, 0x040040440c202840ULL,
    0x3040284404410884ULL, 0x0411082020020088ULL, 0x0210108202221410ULL, 0x9108124040410420ULL, 0x004800040a414800ULL,
    0x000c000200a20000ULL, 0x8000801410008800ULL, 0x4024c1040c04b400ULL, 0x8008841100880100ULL, 0x0420060210100201ULL,
    0x0828600808020088ULL, 0x0000480010008210ULL, 0x4120082001040080ULL, 0x0001010000104000ULL, 0x021202810c100480ULL,
    0x9009440020420840ULL, 0x000090204a020200ULL, 0x0002090402401044ULL, 0x0001301000120480ULL, 0x0010403002081640ULL,
    0x2010020080280080ULL, 0x2100520021020080ULL, 0x2820488100308042ULL, 0x0001020084041400ULL, 0x41010a0024020100ULL,
    0x600824020800a081ULL, 0x0021823820241240ULL, 0x0012020022009400ULL, 0x8020004208007080ULL, 0x400c881014000040ULL,
    0x0570501000404022ULL, 0x08081004024204a8ULL, 0x0001082081000480ULL, 0x1822020104c00000ULL, 0x0002288410280002ULL,
    0x2080010402190801ULL, 0xc200008020880000ULL, 0x2020002861090009ULL, 0x1c09052024010004ULL, 0x0010900200840004ULL,
    0x0010020089220000ULL, 0x0004808490212008ULL, 0x410610a405043008ULL, 0x2000508108411004ULL, 0x1008c14091420880ULL,
    0x00000801d1020a13ULL, 0x00402008310d0209ULL, 0x02001002b0041083ULL, 0x0021200410484240ULL
};

constexpr std::array<int8_t, 8> KnightSteps = {UpRight + Up,    UpRight + Right, DownRight + Right, DownRight + Down,
                                               DownLeft + Down, DownLeft + Left, UpLeft + Left,     UpLeft + Up};
constexpr std::array<int8_t, 8> KingSteps = {Up, UpRight, Right, DownRight, Down, DownLeft, Left, UpLeft};
constexpr std::array<int8_t, 2> WhitePawnSteps = {UpLeft, UpRight};
constexpr std::array<int8_t, 2> BlackPawnSteps = {DownLeft, DownRight};
constexpr std::array<Direction, 4> RookDirections = {Up, Right, Down, Left};
constexpr std::array<Direction, 4> BishopDirections = {UpRight, DownRight, DownLeft, UpLeft};

}; // namespace

constexpr std::array<Bitboard, SquareCNT> KnightAttacks = stepping_attacks(KnightSteps);
constexpr std::array<Bitboard, SquareCNT> KingAttacks = stepping_attacks(KingSteps);
constexpr std::array<Bitboard, SquareCNT> WhitePawnAttacks = stepping_attacks(WhitePawnSteps);
constexpr std::array<Bitboard, SquareCNT> BlackPawnAttacks = stepping_attacks(BlackPawnSteps);

constexpr std::array<MagicInfo, SquareCNT> RookMagics = magic_infos(RookMagicNumbers, RookDirections);
constexpr std::array<MagicInfo, SquareCNT> BishopMagics = magic_infos(BishopMagicNumbers, BishopDirections);
constexpr std::array<Bitboard, 102400> RookAttacks = sliding_attacks<102400>(RookMagics, RookDirections);
constexpr std::array<Bitboard, 5248> BishopAttacks = sliding_attacks<5248>(BishopMagics, BishopDirections);

// based on the last table:         index begin              + size
static_assert(RookAttacks.size() == RookMagics.back().offset + (1ULL << RookMagics.back().shift));
static_assert(BishopAttacks.size() == BishopMagics.back().offset + (1ULL << BishopMagics.back().shift));

} // namespace ches::detail
