#pragma once

#include "attack_precompute.hpp"

namespace cheslib {

constexpr Bitboard knight_attacks(Square from);
constexpr Bitboard king_attacks(Square from);
constexpr Bitboard rook_attacks(Square from, Bitboard occupancy);
constexpr Bitboard bishop_attacks(Square from, Bitboard occupancy);

constexpr Bitboard queen_attacks(Square from, Bitboard occupancy) {
    return bishop_attacks(from, occupancy) | rook_attacks(from, occupancy);
}

template <bool IsBlack>
constexpr Bitboard pawn_attacks(Square from);

namespace detail {

inline constexpr std::array<int8_t, 8> KNIGHT_STEPS = {UP_RIGHT + UP,     UP_RIGHT + RIGHT, DOWN_RIGHT + RIGHT,
                                                       DOWN_RIGHT + DOWN, DOWN_LEFT + DOWN, DOWN_LEFT + LEFT,
                                                       UP_LEFT + LEFT,    UP_LEFT + UP};
inline constexpr std::array<int8_t, 8> KING_STEPS = {UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT};
inline constexpr std::array<int8_t, 2> WHITE_PAWN_STEPS = {UP_LEFT, UP_RIGHT};
inline constexpr std::array<int8_t, 2> BLACK_PAWN_STEPS = {DOWN_LEFT, DOWN_RIGHT};
inline constexpr std::array<Direction, 4> ROOK_DIRECTIONS = {UP, RIGHT, DOWN, LEFT};
inline constexpr std::array<Direction, 4> BISHOP_DIRECTIONS = {UP_RIGHT, DOWN_RIGHT, DOWN_LEFT, UP_LEFT};

inline constexpr std::array<Bitboard, SQUARE_CNT> ROOK_MASKS = sliding_blockers(ROOK_DIRECTIONS);
inline constexpr std::array<Bitboard, SQUARE_CNT> BISHOP_MASKS = sliding_blockers(BISHOP_DIRECTIONS);
inline constexpr std::array<uint8_t, SQUARE_CNT> ROOK_SHIFTS = popcounts(ROOK_MASKS);
inline constexpr std::array<uint8_t, SQUARE_CNT> BISHOP_SHIFTS = popcounts(BISHOP_MASKS);
inline constexpr std::array<uint32_t, SQUARE_CNT> ROOK_OFFSETS = occupancy_offsets(ROOK_SHIFTS);
inline constexpr std::array<uint32_t, SQUARE_CNT> BISHOP_OFFSETS = occupancy_offsets(BISHOP_SHIFTS);

// generated on 2026-04-05 using attack_precompute.hpp's find_magic()
// the numbers mason what do they mean????
inline constexpr std::array<uint64_t, SQUARE_CNT> ROOK_MAGICS = {
    0x0080008020400010ULL, 0x0440400020001000ULL, 0x0280100081200008ULL, 0x0480100004080080ULL, 0x0200080200041020ULL,
    0x9100010004006802ULL, 0x0400080082010430ULL, 0x0200150480402402ULL, 0x0099800020400281ULL, 0x8000402000401000ULL,
    0x2041002008110040ULL, 0x0104800800100580ULL, 0x4002800800804402ULL, 0x0501000400080300ULL, 0x4004000210010448ULL,
    0x4042000531420084ULL, 0x50018180004000a8ULL, 0x8000820021020046ULL, 0x0110008080102000ULL, 0x0008808010040800ULL,
    0x0008010010080500ULL, 0x0930080104402010ULL, 0x0200010100040200ULL, 0x0070020014004091ULL, 0x00a8400480002081ULL,
    0x4901008200402200ULL, 0x2020008080201001ULL, 0x1440080080801000ULL, 0x1100050100100800ULL, 0x0c20040080800200ULL,
    0x0001000100020004ULL, 0x1450408200004401ULL, 0x1000400088800430ULL, 0x24c0100800202000ULL, 0x0034814012002200ULL,
    0x0400880084801000ULL, 0x4008000880800400ULL, 0x88a2810200800400ULL, 0x000028d004000122ULL, 0x000008b106000044ULL,
    0x1808410200860024ULL, 0x5010200050004004ULL, 0x0680200010008080ULL, 0x20020040200a0014ULL, 0x4000080100050010ULL,
    0x82b2001020040400ULL, 0x0000040200010100ULL, 0x1020004081020004ULL, 0xa020284004800680ULL, 0x8020004001300140ULL,
    0x028a200011004100ULL, 0x0010048012080080ULL, 0x0800800400080080ULL, 0x0004010002004040ULL, 0x4881000200040100ULL,
    0x0150208411004200ULL, 0x0008204080160302ULL, 0x4009604000148305ULL, 0x0408406002181103ULL, 0x024600042011c00aULL,
    0x0801000402100801ULL, 0x024b000a04000805ULL, 0x0240008241081004ULL, 0x0010040900402082ULL,
};
inline constexpr std::array<uint64_t, SQUARE_CNT> BISHOP_MAGICS = {
    0xa042202822008021ULL, 0x10200200c5010882ULL, 0x0522080100210000ULL, 0x10680a0221000000ULL, 0x2011104000800802ULL,
    0x00010120100900a0ULL, 0x080048043008820eULL, 0x0042020602220202ULL, 0x0080106018a10044ULL, 0x8100080801141022ULL,
    0x014804081a0040d8ULL, 0x1204844400800000ULL, 0x0000020211108400ULL, 0x80042104a0040000ULL, 0x1412004202104000ULL,
    0x0002408041082020ULL, 0x1108002082500245ULL, 0xc0042082302c4108ULL, 0xc148001000801010ULL, 0x1402021028220050ULL,
    0x0801021820082142ULL, 0x0041000290009001ULL, 0x0004080108888403ULL, 0x000a018022012400ULL, 0x24024004a0290a40ULL,
    0x0c10040208012400ULL, 0x0800404204040084ULL, 0x91380800008a0020ULL, 0x8001020084008404ULL, 0x4208002002008420ULL,
    0x1088405000841400ULL, 0x28010100022c0124ULL, 0x2110301080140d22ULL, 0x0001080850200151ULL, 0x5202044040040100ULL,
    0x2000840400080210ULL, 0x00540284000a0500ULL, 0x0801110202050800ULL, 0x20010a1400008438ULL, 0x0611004301160502ULL,
    0x4414020240c81100ULL, 0x0005080110200500ULL, 0x4020840401040200ULL, 0x800459a204200800ULL, 0x0a00502014420200ULL,
    0x0020040088240a00ULL, 0x142001420880c21aULL, 0x42010a6201c01200ULL, 0x0112080208660024ULL, 0x0020232402204011ULL,
    0x0028002601104100ULL, 0x0800024884041880ULL, 0x2000024005010000ULL, 0x8020200401420018ULL, 0x2810100218106001ULL,
    0x0082040404104242ULL, 0x1010110090042000ULL, 0x0068a50101304245ULL, 0x0002000202010410ULL, 0x0142004200840400ULL,
    0x0424200210020210ULL, 0x8431602102024200ULL, 0x0402408802242052ULL, 0x04e0028088048080ULL,

};

// based on last occupancy:  index begin         + size
inline constexpr size_t ROOK_SIZE = ROOK_OFFSETS.back() + (1ULL << ROOK_SHIFTS.back());
inline constexpr size_t BISHOP_SIZE = BISHOP_OFFSETS.back() + (1ULL << BISHOP_SHIFTS.back());

inline constexpr std::array<Bitboard, ROOK_SIZE> ROOK_ATTACKS =
    sliding_attacks<ROOK_SIZE>(ROOK_MASKS, ROOK_MAGICS, ROOK_SHIFTS, ROOK_OFFSETS, ROOK_DIRECTIONS);
inline constexpr std::array<Bitboard, BISHOP_SIZE> BISHOP_ATTACKS =
    sliding_attacks<BISHOP_SIZE>(BISHOP_MASKS, BISHOP_MAGICS, BISHOP_SHIFTS, BISHOP_OFFSETS, BISHOP_DIRECTIONS);

inline constexpr std::array<Bitboard, SQUARE_CNT> KNIGHT_ATTACKS = stepping_attacks(KNIGHT_STEPS);
inline constexpr std::array<Bitboard, SQUARE_CNT> KING_ATTACKS = stepping_attacks(KING_STEPS);
inline constexpr std::array<Bitboard, SQUARE_CNT> WHITE_PAWN_ATTACKS = stepping_attacks(WHITE_PAWN_STEPS);
inline constexpr std::array<Bitboard, SQUARE_CNT> BLACK_PAWN_ATTACKS = stepping_attacks(BLACK_PAWN_STEPS);

} // namespace detail

constexpr Bitboard knight_attacks(Square from) {
    assert(from < SQUARE_CNT);
    return detail::KNIGHT_ATTACKS[from];
}

constexpr Bitboard king_attacks(Square from) {
    assert(from < SQUARE_CNT);
    return detail::KING_ATTACKS[from];
}

constexpr Bitboard rook_attacks(Square from, Bitboard occupancy) {
    assert(from < SQUARE_CNT);

    Bitboard mask = detail::ROOK_MASKS[from];
    uint64_t magic = detail::ROOK_MAGICS[from];
    uint8_t shift = detail::ROOK_SHIFTS[from];
    uint32_t offset = detail::ROOK_OFFSETS[from];

    size_t index = detail::magic_index(occupancy, mask, magic, shift);
    return detail::ROOK_ATTACKS[offset + index];
}

constexpr Bitboard bishop_attacks(Square from, Bitboard occupancy) {
    assert(from < SQUARE_CNT);

    Bitboard mask = detail::BISHOP_MASKS[from];
    uint64_t magic = detail::BISHOP_MAGICS[from];
    uint8_t shift = detail::BISHOP_SHIFTS[from];
    uint32_t offset = detail::BISHOP_OFFSETS[from];

    size_t index = detail::magic_index(occupancy, mask, magic, shift);
    return detail::BISHOP_ATTACKS[offset + index];
}

template <bool IsBlack>
constexpr Bitboard pawn_attacks(Square from) {
    assert(from < SQUARE_CNT);
    return IsBlack ? detail::BLACK_PAWN_ATTACKS[from] : detail::WHITE_PAWN_ATTACKS[from];
}

} // namespace cheslib
