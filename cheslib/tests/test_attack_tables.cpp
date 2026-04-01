#include <catch2/catch_test_macros.hpp>

#include "attack_tables.hpp"
#include "cheslib/types.hpp"

namespace cheslib_test {

using namespace cheslib;

namespace {

constexpr bool is_set(Bitboard bb, Square sq) {
    return bb & (1ULL << sq);
}

constexpr Bitboard make_bitboard(std::initializer_list<Square> squares) {
    Bitboard bb = 0;
    for (Square sq : squares) {
        bb |= (1ULL << sq);
    }
    return bb;
}

} // namespace

TEST_CASE("Pieces stepping attacks", "[attack_tables]") {
    SECTION("Knight") {
        // corners
        CHECK(knight_attacks(SQUARE_A1) == make_bitboard({SQUARE_B3, SQUARE_C2}));
        CHECK(knight_attacks(SQUARE_D1) == make_bitboard({SQUARE_B2, SQUARE_C3, SQUARE_E3, SQUARE_F2}));

        // center
        CHECK(std::popcount(knight_attacks(SQUARE_E4)) == 8);
    }

    SECTION("King") {
        // corner
        CHECK(king_attacks(SQUARE_H8) == make_bitboard({SQUARE_G8, SQUARE_G7, SQUARE_H7}));

        // center
        CHECK(
            king_attacks(SQUARE_D4) ==
            make_bitboard({SQUARE_C3, SQUARE_C4, SQUARE_C5, SQUARE_D3, SQUARE_D5, SQUARE_E3, SQUARE_E4, SQUARE_E5})
        );
    }

    SECTION("Pawn") {
        // edge files
        CHECK(white_pawn_attacks(SQUARE_A2) == make_bitboard({SQUARE_B3}));
        CHECK(black_pawn_attacks(SQUARE_H7) == make_bitboard({SQUARE_G6}));

        // center
        CHECK(white_pawn_attacks(SQUARE_E4) == make_bitboard({SQUARE_D5, SQUARE_F5}));
        CHECK(black_pawn_attacks(SQUARE_E5) == make_bitboard({SQUARE_D4, SQUARE_F4}));

        // back ranks
        CHECK(white_pawn_attacks(SQUARE_A8) == 0);
        CHECK(black_pawn_attacks(SQUARE_A1) == 0);
    }
}

TEST_CASE("Rook sliding attacks", "[attack_tables]") {
    SECTION("Empty board") {
        Bitboard attacks = rook_attacks(SQUARE_D4, 0);
        CHECK(std::popcount(attacks) == 14); // 14 attack squares
        CHECK(is_set(attacks, SQUARE_D1));
        CHECK(is_set(attacks, SQUARE_D8));
        CHECK(is_set(attacks, SQUARE_A4));
        CHECK(is_set(attacks, SQUARE_H4));
    }

    SECTION("Blockers") {
        Bitboard blockers = make_bitboard({SQUARE_D6, SQUARE_B4});
        Bitboard bb = rook_attacks(SQUARE_D4, blockers);

        // upward stops at D6
        CHECK(is_set(bb, SQUARE_D5));
        CHECK(is_set(bb, SQUARE_D6));
        CHECK(!is_set(bb, SQUARE_D7));

        // left stops at B4
        CHECK(is_set(bb, SQUARE_C4));
        CHECK(is_set(bb, SQUARE_B4));
        CHECK(!is_set(bb, SQUARE_A4));

        // edge squares
        CHECK(is_set(bb, SQUARE_H4));
        CHECK(is_set(bb, SQUARE_D1));
    }
}

TEST_CASE("Bishop sliding attacks", "[attack_tables]") {
    SECTION("Empty board") {
        Bitboard bb = bishop_attacks(SQUARE_D4, 0);
        CHECK(std::popcount(bb) == 13);
        CHECK(is_set(bb, SQUARE_A1));
        CHECK(is_set(bb, SQUARE_G7));
    }

    SECTION("Blockers") {
        Bitboard blockers = 1ULL << SQUARE_B2;
        Bitboard bb = bishop_attacks(SQUARE_A1, blockers);
        CHECK(is_set(bb, SQUARE_B2));
        CHECK(!is_set(bb, SQUARE_A1));
        CHECK(!is_set(bb, SQUARE_B3));

        // F6 surrounded
        blockers = make_bitboard({SQUARE_E5, SQUARE_E7, SQUARE_G5, SQUARE_G7});
        bb = bishop_attacks(SQUARE_F6, blockers);
        for (Square sq : {SQUARE_D4, SQUARE_D8, SQUARE_H4, SQUARE_H8}) {
            CHECK(!is_set(bb, sq));
        }
    }
}

TEST_CASE("Constexpr checks", "[attack_tables]") {
    static_assert(std::popcount(knight_attacks(SQUARE_E4)) == 8);
    static_assert(white_pawn_attacks(SQUARE_B2) == make_bitboard({SQUARE_A3, SQUARE_C3}));
    static_assert(std::popcount(rook_attacks(SQUARE_A1, 0)) == 14);
}

} // namespace cheslib_test
