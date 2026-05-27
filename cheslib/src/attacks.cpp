#include <array>
#include <span>

#include "attacks.hpp"

namespace cheslib::attacks {

namespace {

struct Magic {
    Bitboard mask;
    uint64_t magic;
    uint32_t offset;
    uint32_t shift;

    constexpr size_t index(Bitboard occupancy) const {
        return offset + (((occupancy & mask) * magic) >> shift);
    }
};

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

consteval std::array<Magic, SquareCNT> magic_infos(
    std::span<const Direction> directions, const uint64_t magic_numbers[SquareCNT]
) {
    std::array<Magic, SquareCNT> result = {};

    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        uint32_t offset;

        if (sq == SquareA1) {
            offset = 0;
        } else {
            uint32_t prev_table_size = 1ull << std::popcount(result[sq - 1].mask);
            offset = result[sq - 1].offset + prev_table_size;
        }

        Bitboard mask = sliding_blockers(sq, directions);
        uint64_t magic = magic_numbers[sq];
        uint32_t shift = 64 - std::popcount(mask);

        result[sq] = Magic{mask, magic, offset, shift};
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
template <size_t Size>
consteval std::array<Bitboard, Size> sliding_attacks(
    const std::array<Magic, SquareCNT> &magics, std::span<const Direction> directions
) {
    std::array<Bitboard, Size> result = {};

    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        const Magic &magic = magics[sq];
        Bitboard occupancy = 0;

        // iterate all occupancy subsets
        // see: https://www.chessprogramming.org/Traversing_Subsets_of_a_Set
        do {
            size_t index = magic.index(occupancy);
            result[index] = sliding_attack_at(sq, occupancy, directions);

            occupancy = (occupancy - magic.mask) & magic.mask;
        } while (occupancy);
    }

    return result;
}

constexpr int8_t KnightSteps[8] = {NorthEast + North, NorthEast + East, SouthEast + East, SouthEast + South,
                                   SouthWest + South, SouthWest + West, NorthWest + West, NorthWest + North};
constexpr int8_t KingSteps[8] = {North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest};
constexpr int8_t WhitePawnSteps[2] = {NorthWest, NorthEast};
constexpr int8_t BlackPawnSteps[2] = {SouthWest, SouthEast};
constexpr Direction RookDirections[4] = {North, East, South, West};
constexpr Direction BishopDirections[4] = {NorthEast, SouthEast, SouthWest, NorthWest};

constexpr std::array<Bitboard, SquareCNT> KnightAttacks = stepping_attacks(KnightSteps);
constexpr std::array<Bitboard, SquareCNT> KingAttacks = stepping_attacks(KingSteps);
constexpr std::array<Bitboard, SquareCNT> PawnAttacks[2] = {
    stepping_attacks(WhitePawnSteps), stepping_attacks(BlackPawnSteps)
};

constexpr uint64_t RookMagicNumbers[SquareCNT] = {
    0x0080008040002010, 0x22c0042000100040, 0x8080100020008008, 0xc100100100080421, 0x4080020800810400,
    0x2100040001000802, 0x4400020110b01804, 0x0200088200412c03, 0x2016800120400288, 0x1108808020004000,
    0x0240802000100080, 0x9281000900100020, 0x0006001200042008, 0x0042000200081004, 0x1046000811020004,
    0x2014801041000080, 0x0020308000400080, 0x000d020044802204, 0x0840808010002004, 0x0010004008040040,
    0x0802020008218410, 0x0c41010002080400, 0x0000040041500208, 0x4000020000806401, 0x0480004240016002,
    0x2001200280400080, 0x0100110100402000, 0x4080080080100084, 0x0044000480800800, 0x1142000404002010,
    0x0001000100020004, 0x8a1000460004008d, 0x02c0085080800020, 0x4010804008802000, 0x0300802000801000,
    0x2000100282800800, 0x1000800400800800, 0x080c800200800400, 0x1086002412000829, 0x0000004082002104,
    0x7028882040008000, 0x0000810022020040, 0x0020020400101000, 0x10120040a1120008, 0x2808002040040400,
    0x0202011058220004, 0x1009000a00150004, 0x804c8680cc220001, 0x0802004284350200, 0x2200802100400100,
    0x00200080a0100180, 0x1010002208110100, 0x000a800400080080, 0x111a008004000280, 0x8000a12802104400,
    0x0100040840a10200, 0xa442004821001082, 0x224200e101104282, 0x9009004008102001, 0x0000100005002109,
    0x1012002008100402, 0x0005004400288201, 0x0120080630088504, 0x0000088c00204102
};

constexpr uint64_t BishopMagicNumbers[SquareCNT] = {
    0x202024a480810200, 0x8104040800410401, 0x8004194202004102, 0x9008204040000200, 0x0308484004403400,
    0x0402180208040440, 0x0311011010840200, 0x0040184804100800, 0x0010400208420c81, 0x0120101091204080,
    0xa08090140940d880, 0x2100844401801093, 0x0002040420100080, 0x220002080208c000, 0x040040440c202840,
    0x3040284404410884, 0x0411082020020088, 0x0210108202221410, 0x9108124040410420, 0x004800040a414800,
    0x000c000200a20000, 0x8000801410008800, 0x4024c1040c04b400, 0x8008841100880100, 0x0420060210100201,
    0x0828600808020088, 0x0000480010008210, 0x4120082001040080, 0x0001010000104000, 0x021202810c100480,
    0x9009440020420840, 0x000090204a020200, 0x0002090402401044, 0x0001301000120480, 0x0010403002081640,
    0x2010020080280080, 0x2100520021020080, 0x2820488100308042, 0x0001020084041400, 0x41010a0024020100,
    0x600824020800a081, 0x0021823820241240, 0x0012020022009400, 0x8020004208007080, 0x400c881014000040,
    0x0570501000404022, 0x08081004024204a8, 0x0001082081000480, 0x1822020104c00000, 0x0002288410280002,
    0x2080010402190801, 0xc200008020880000, 0x2020002861090009, 0x1c09052024010004, 0x0010900200840004,
    0x0010020089220000, 0x0004808490212008, 0x410610a405043008, 0x2000508108411004, 0x1008c14091420880,
    0x00000801d1020a13, 0x00402008310d0209, 0x02001002b0041083, 0x0021200410484240
};

constexpr std::array<Magic, SquareCNT> RookMagics = magic_infos(RookDirections, RookMagicNumbers);
constexpr std::array<Magic, SquareCNT> BishopMagics = magic_infos(BishopDirections, BishopMagicNumbers);

constexpr std::array<Bitboard, 102400> RookAttacks = sliding_attacks<102400>(RookMagics, RookDirections);
constexpr std::array<Bitboard, 5248> BishopAttacks = sliding_attacks<5248>(BishopMagics, BishopDirections);

// based on the last table:         index begin           + size
static_assert(RookAttacks.size() == RookMagics[63].offset + (1ull << std::popcount(RookMagics[63].mask)));
static_assert(BishopAttacks.size() == BishopMagics[63].offset + (1ull << std::popcount(BishopMagics[63].mask)));

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
    size_t index = RookMagics[from].index(occupancy);
    return RookAttacks[index];
}

Bitboard bishop(Square from, Bitboard occupancy) {
    assert(from < SquareCNT);
    size_t index = BishopMagics[from].index(occupancy);
    return BishopAttacks[index];
}

Bitboard queen(Square from, Bitboard occupancy) {
    return bishop(from, occupancy) | rook(from, occupancy);
}

} // namespace cheslib::attacks
