#include <catch2/catch_test_macros.hpp>

#include "attack_precompute.hpp"
#include "attack_tables.hpp"
#include "cheslib/types.hpp"
#include "utils.hpp"

namespace cheslib_test {

using namespace cheslib;

TEST_CASE("Pieces stepping attacks", "[attack_tables]") {
    SECTION("Knight") {
        // corners
        CHECK(knight_attacks(SQUARE_A1) == make_bitboard(SQUARE_B3, SQUARE_C2));
        CHECK(knight_attacks(SQUARE_D1) == make_bitboard(SQUARE_B2, SQUARE_C3, SQUARE_E3, SQUARE_F2));

        // center
        CHECK(std::popcount(knight_attacks(SQUARE_E4)) == 8);
    }

    SECTION("King") {
        // corner
        CHECK(king_attacks(SQUARE_H8) == make_bitboard(SQUARE_G8, SQUARE_G7, SQUARE_H7));

        // center
        CHECK(
            king_attacks(SQUARE_D4) ==
            make_bitboard(SQUARE_C3, SQUARE_C4, SQUARE_C5, SQUARE_D3, SQUARE_D5, SQUARE_E3, SQUARE_E4, SQUARE_E5)
        );
    }

    SECTION("Pawn") {
        constexpr bool black = true;
        constexpr bool white = !black;

        // edge files
        CHECK(pawn_attacks<white>(SQUARE_A2) == make_bitboard(SQUARE_B3));
        CHECK(pawn_attacks<black>(SQUARE_H7) == make_bitboard(SQUARE_G6));

        // center
        CHECK(pawn_attacks<white>(SQUARE_E4) == make_bitboard(SQUARE_D5, SQUARE_F5));
        CHECK(pawn_attacks<black>(SQUARE_E5) == make_bitboard(SQUARE_D4, SQUARE_F4));

        // back ranks
        CHECK(pawn_attacks<white>(SQUARE_A8) == 0);
        CHECK(pawn_attacks<black>(SQUARE_A1) == 0);
    }
}

TEST_CASE("Rook sliding attacks", "[attack_tables]") {
    SECTION("Empty board") {
        Bitboard attacks = rook_attacks(SQUARE_D4, 0);
        CHECK(std::popcount(attacks) == 14); // 14 attack squares
        CHECK(has_square(attacks, SQUARE_D1));
        CHECK(has_square(attacks, SQUARE_D8));
        CHECK(has_square(attacks, SQUARE_A4));
        CHECK(has_square(attacks, SQUARE_H4));
    }

    SECTION("Blockers") {
        Bitboard blockers = make_bitboard(SQUARE_D6, SQUARE_B4);
        Bitboard bb = rook_attacks(SQUARE_D4, blockers);

        // upward stops at D6
        CHECK(has_square(bb, SQUARE_D5));
        CHECK(has_square(bb, SQUARE_D6));
        CHECK(!has_square(bb, SQUARE_D7));

        // left stops at B4
        CHECK(has_square(bb, SQUARE_C4));
        CHECK(has_square(bb, SQUARE_B4));
        CHECK(!has_square(bb, SQUARE_A4));

        // edge squares
        CHECK(has_square(bb, SQUARE_H4));
        CHECK(has_square(bb, SQUARE_D1));
    }
}

TEST_CASE("Bishop sliding attacks", "[attack_tables]") {
    SECTION("Empty board") {
        Bitboard bb = bishop_attacks(SQUARE_D4, 0);
        CHECK(std::popcount(bb) == 13);
        CHECK(has_square(bb, SQUARE_A1));
        CHECK(has_square(bb, SQUARE_G7));
    }

    SECTION("Blockers") {
        Bitboard blockers = 1ULL << SQUARE_B2;
        Bitboard bb = bishop_attacks(SQUARE_A1, blockers);
        CHECK(has_square(bb, SQUARE_B2));
        CHECK(!has_square(bb, SQUARE_A1));
        CHECK(!has_square(bb, SQUARE_B3));

        // F6 surrounded
        blockers = make_bitboard(SQUARE_E5, SQUARE_E7, SQUARE_G5, SQUARE_G7);
        bb = bishop_attacks(SQUARE_F6, blockers);
        for (Square sq : {SQUARE_D4, SQUARE_D8, SQUARE_H4, SQUARE_H8}) {
            CHECK(!has_square(bb, sq));
        }
    }
}

TEST_CASE("Constexpr checks", "[attack_tables]") {
    static_assert(std::popcount(knight_attacks(SQUARE_E4)) == 8);
    static_assert(std::popcount(king_attacks(SQUARE_H1)) == 3);
    static_assert(pawn_attacks<false>(SQUARE_B2) == make_bitboard(SQUARE_A3, SQUARE_C3));
    static_assert(std::popcount(rook_attacks(SQUARE_A1, 0)) == 14);
    static_assert(std::popcount(bishop_attacks(SQUARE_H2, 0)) == 7);
    static_assert(std::popcount(queen_attacks(SQUARE_D5, 0)) == 27);
}

TEST_CASE("Sliding attacks random occupancy", "[attack_tables]") {
    using namespace cheslib::detail;

    Bitboard occupancy = 0x123456789ABCDEF0ULL;
    auto reroll = [&occupancy]() {
        occupancy ^= occupancy << 13;
        occupancy ^= occupancy >> 7;
        occupancy ^= occupancy << 17;
    };

    SECTION("Rook") {
        for (Square sq = SQUARE_A1; sq < SQUARE_CNT; ++sq) {
            Bitboard expected = sliding_attack_at(sq, occupancy, ROOK_DIRECTIONS);
            CHECK(rook_attacks(sq, occupancy) == expected);
            reroll();
        }
    }

    SECTION("Bishop") {
        for (Square sq = SQUARE_A1; sq < SQUARE_CNT; ++sq) {
            Bitboard expected = sliding_attack_at(sq, occupancy, BISHOP_DIRECTIONS);
            CHECK(bishop_attacks(sq, occupancy) == expected);
            reroll();
        }
    }
}

} // namespace cheslib_test
