#include <catch2/catch_test_macros.hpp>

#include "attacks.hpp"

using namespace ::cheslib;

static void check_has_squares(Bitboard bb, std::initializer_list<Square> squares) {
    for (Square sq : squares) {
        CHECK(types::has_square(bb, sq));
    }
}

TEST_CASE("Attacks: Check pieces stepping Attacks", "[attacks]") {
    SECTION("Knight") {
        // corners
        CHECK(attacks::knight(SquareA1) == types::bitboard_of(SquareB3, SquareC2));
        CHECK(attacks::knight(SquareD1) == types::bitboard_of(SquareB2, SquareC3, SquareE3, SquareF2));

        // center
        CHECK(std::popcount(attacks::knight(SquareE4)) == 8);
    }

    SECTION("King") {
        // corner
        CHECK(attacks::king(SquareH8) == types::bitboard_of(SquareG8, SquareG7, SquareH7));

        // center
        Bitboard expected =
            types::bitboard_of(SquareC3, SquareC4, SquareC5, SquareD3, SquareD5, SquareE3, SquareE4, SquareE5);
        CHECK(attacks::king(SquareD4) == expected);
    }

    SECTION("Pawn") {
        // edge files
        CHECK(attacks::pawn(SquareA2, White) == types::bitboard_of(SquareB3));
        CHECK(attacks::pawn(SquareH7, Black) == types::bitboard_of(SquareG6));

        // center
        CHECK(attacks::pawn(SquareE4, White) == types::bitboard_of(SquareD5, SquareF5));
        CHECK(attacks::pawn(SquareE5, Black) == types::bitboard_of(SquareD4, SquareF4));

        // back ranks
        CHECK(attacks::pawn(SquareA8, White) == 0);
        CHECK(attacks::pawn(SquareA1, Black) == 0);
    }
}

TEST_CASE("Attacks: Check rook sliding attack magic table", "[attacks]") {
    SECTION("Empty board") {
        Bitboard attacks = attacks::rook(SquareD4, 0);
        CHECK(std::popcount(attacks) == 14); // 14 attack squares
        check_has_squares(attacks, {SquareD1, SquareD8, SquareA4, SquareH4});
    }

    SECTION("Corner on empty board") {
        Bitboard attacks = attacks::rook(SquareA1, 0);
        CHECK(std::popcount(attacks) == 14);
        check_has_squares(attacks, {SquareA8, SquareH1});
    }

    SECTION("Blockers") {
        Bitboard blockers = types::bitboard_of(SquareD6, SquareB4);
        Bitboard bb = attacks::rook(SquareD4, blockers);

        // upward stops at D6
        check_has_squares(bb, {SquareD5, SquareD6});
        CHECK_FALSE(types::has_square(bb, SquareD7));

        // left stops at B4
        check_has_squares(bb, {SquareC4, SquareB4});
        CHECK_FALSE(types::has_square(bb, SquareA4));

        // edge squares
        check_has_squares(bb, {SquareH4, SquareD1});
    }

    SECTION("Immediate orthogonal blockers") {
        Bitboard blockers = types::bitboard_of(SquareD5, SquareD3, SquareC4, SquareE4);
        Bitboard bb = attacks::rook(SquareD4, blockers);
        CHECK(bb == types::bitboard_of(SquareD5, SquareD3, SquareC4, SquareE4));
    }

    SECTION("First blocker on ray limits farther blockers") {
        Bitboard blockers = types::bitboard_of(SquareD6, SquareD7, SquareB4);
        Bitboard bb = attacks::rook(SquareD4, blockers);

        check_has_squares(bb, {SquareD5, SquareD6});
        CHECK_FALSE(types::has_square(bb, SquareD7));
        CHECK_FALSE(types::has_square(bb, SquareD8));

        check_has_squares(bb, {SquareC4, SquareB4});
        CHECK_FALSE(types::has_square(bb, SquareA4));
    }
}

TEST_CASE("Attacks: Check bishop sliding attack magic table", "[attacks]") {
    SECTION("Empty board") {
        Bitboard bb = attacks::bishop(SquareD4, 0);
        CHECK(std::popcount(bb) == 13);
        check_has_squares(bb, {SquareA1, SquareG7, SquareA7, SquareG1});
    }

    SECTION("Corner on empty board") {
        Bitboard bb = attacks::bishop(SquareH1, 0);
        CHECK(std::popcount(bb) == 7);
        check_has_squares(bb, {SquareG2, SquareF3, SquareE4, SquareD5, SquareC6, SquareB7, SquareA8});
    }

    SECTION("Blockers") {
        Bitboard blockers = 1ULL << SquareB2;
        Bitboard bb = attacks::bishop(SquareA1, blockers);
        CHECK(types::has_square(bb, SquareB2));
        CHECK_FALSE(types::has_square(bb, SquareA1));
        CHECK_FALSE(types::has_square(bb, SquareB3));

        // F6 surrounded
        blockers = types::bitboard_of(SquareE5, SquareE7, SquareG5, SquareG7);
        bb = attacks::bishop(SquareF6, blockers);
        for (Square sq : {SquareD4, SquareD8, SquareH4, SquareH8}) {
            CHECK_FALSE(types::has_square(bb, sq));
        }
    }

    SECTION("Immediate diagonal blockers") {
        Bitboard blockers = types::bitboard_of(SquareC5, SquareE5, SquareC3, SquareE3);
        Bitboard bb = attacks::bishop(SquareD4, blockers);
        CHECK(bb == types::bitboard_of(SquareC5, SquareE5, SquareC3, SquareE3));
    }

    SECTION("First blocker on diagonal limits farther blockers") {
        Bitboard blockers = types::bitboard_of(SquareE3, SquareF4, SquareB2);
        Bitboard bb = attacks::bishop(SquareC1, blockers);

        check_has_squares(bb, {SquareD2, SquareE3, SquareB2});
        CHECK_FALSE(types::has_square(bb, SquareF4));
        CHECK_FALSE(types::has_square(bb, SquareA3));
    }
}
