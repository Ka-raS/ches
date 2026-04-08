#include <array>

#include <catch2/catch_test_macros.hpp>

#include "cheslib/types.hpp"

#include "attack_tables.hpp"
#include "utils.hpp"

TEST_CASE("Check pieces stepping attack tables", "[attack_tables]") {
    SECTION("Knight") {
        // corners
        CHECK(ches::knight_attacks(ches::SquareA1) == to_bitboard(ches::SquareB3, ches::SquareC2));
        CHECK(
            ches::knight_attacks(ches::SquareD1) ==
            to_bitboard(ches::SquareB2, ches::SquareC3, ches::SquareE3, ches::SquareF2)
        );

        // center
        CHECK(std::popcount(ches::knight_attacks(ches::SquareE4)) == 8);
    }

    SECTION("King") {
        // corner
        CHECK(ches::king_attacks(ches::SquareH8) == to_bitboard(ches::SquareG8, ches::SquareG7, ches::SquareH7));

        // center
        ches::Bitboard expected = to_bitboard(
            ches::SquareC3, ches::SquareC4, ches::SquareC5, ches::SquareD3, ches::SquareD5, ches::SquareE3,
            ches::SquareE4, ches::SquareE5
        );
        CHECK(ches::king_attacks(ches::SquareD4) == expected);
    }

    SECTION("Pawn") {
        // edge files
        CHECK(ches::pawn_attacks<ches::White>(ches::SquareA2) == to_bitboard(ches::SquareB3));
        CHECK(ches::pawn_attacks<ches::Black>(ches::SquareH7) == to_bitboard(ches::SquareG6));

        // center
        CHECK(ches::pawn_attacks<ches::White>(ches::SquareE4) == to_bitboard(ches::SquareD5, ches::SquareF5));
        CHECK(ches::pawn_attacks<ches::Black>(ches::SquareE5) == to_bitboard(ches::SquareD4, ches::SquareF4));

        // back ranks
        CHECK(ches::pawn_attacks<ches::White>(ches::SquareA8) == 0);
        CHECK(ches::pawn_attacks<ches::Black>(ches::SquareA1) == 0);
    }
}

TEST_CASE("Check rook sliding attack magic table", "[attack_tables]") {
    SECTION("Empty board") {
        ches::Bitboard attacks = ches::rook_attacks(ches::SquareD4, 0);
        CHECK(std::popcount(attacks) == 14); // 14 attack squares
        CHECK(ches::has_square(attacks, ches::SquareD1));
        CHECK(ches::has_square(attacks, ches::SquareD8));
        CHECK(ches::has_square(attacks, ches::SquareA4));
        CHECK(ches::has_square(attacks, ches::SquareH4));
    }

    SECTION("Corner on empty board") {
        ches::Bitboard attacks = ches::rook_attacks(ches::SquareA1, 0);
        CHECK(std::popcount(attacks) == 14);
        CHECK(ches::has_square(attacks, ches::SquareA8));
        CHECK(ches::has_square(attacks, ches::SquareH1));
    }

    SECTION("Blockers") {
        ches::Bitboard blockers = to_bitboard(ches::SquareD6, ches::SquareB4);
        ches::Bitboard bb = ches::rook_attacks(ches::SquareD4, blockers);

        // upward stops at D6
        CHECK(ches::has_square(bb, ches::SquareD5));
        CHECK(ches::has_square(bb, ches::SquareD6));
        CHECK(!ches::has_square(bb, ches::SquareD7));

        // left stops at B4
        CHECK(ches::has_square(bb, ches::SquareC4));
        CHECK(ches::has_square(bb, ches::SquareB4));
        CHECK(!ches::has_square(bb, ches::SquareA4));

        // edge squares
        CHECK(ches::has_square(bb, ches::SquareH4));
        CHECK(ches::has_square(bb, ches::SquareD1));
    }

    SECTION("Immediate orthogonal blockers") {
        ches::Bitboard blockers = to_bitboard(ches::SquareD5, ches::SquareD3, ches::SquareC4, ches::SquareE4);
        ches::Bitboard bb = ches::rook_attacks(ches::SquareD4, blockers);
        CHECK(bb == to_bitboard(ches::SquareD5, ches::SquareD3, ches::SquareC4, ches::SquareE4));
    }

    SECTION("First blocker on ray limits farther blockers") {
        ches::Bitboard blockers = to_bitboard(ches::SquareD6, ches::SquareD7, ches::SquareB4);
        ches::Bitboard bb = ches::rook_attacks(ches::SquareD4, blockers);

        CHECK(ches::has_square(bb, ches::SquareD5));
        CHECK(ches::has_square(bb, ches::SquareD6));
        CHECK(!ches::has_square(bb, ches::SquareD7));
        CHECK(!ches::has_square(bb, ches::SquareD8));

        CHECK(ches::has_square(bb, ches::SquareC4));
        CHECK(ches::has_square(bb, ches::SquareB4));
        CHECK(!ches::has_square(bb, ches::SquareA4));
    }
}

TEST_CASE("Check bishop sliding attack magic table", "[attack_tables]") {
    SECTION("Empty board") {
        ches::Bitboard bb = ches::bishop_attacks(ches::SquareD4, 0);
        CHECK(std::popcount(bb) == 13);
        CHECK(ches::has_square(bb, ches::SquareA1));
        CHECK(ches::has_square(bb, ches::SquareG7));
    }

    SECTION("Corner on empty board") {
        ches::Bitboard bb = ches::bishop_attacks(ches::SquareH1, 0);
        CHECK(std::popcount(bb) == 7);
        CHECK(ches::has_square(bb, ches::SquareG2));
        CHECK(ches::has_square(bb, ches::SquareA8));
    }

    SECTION("Blockers") {
        ches::Bitboard blockers = 1ULL << ches::SquareB2;
        ches::Bitboard bb = ches::bishop_attacks(ches::SquareA1, blockers);
        CHECK(ches::has_square(bb, ches::SquareB2));
        CHECK(!ches::has_square(bb, ches::SquareA1));
        CHECK(!ches::has_square(bb, ches::SquareB3));

        // F6 surrounded
        blockers = to_bitboard(ches::SquareE5, ches::SquareE7, ches::SquareG5, ches::SquareG7);
        bb = ches::bishop_attacks(ches::SquareF6, blockers);
        for (ches::Square sq : {ches::SquareD4, ches::SquareD8, ches::SquareH4, ches::SquareH8}) {
            CHECK(!ches::has_square(bb, sq));
        }
    }

    SECTION("Immediate diagonal blockers") {
        ches::Bitboard blockers = to_bitboard(ches::SquareC5, ches::SquareE5, ches::SquareC3, ches::SquareE3);
        ches::Bitboard bb = ches::bishop_attacks(ches::SquareD4, blockers);
        CHECK(bb == to_bitboard(ches::SquareC5, ches::SquareE5, ches::SquareC3, ches::SquareE3));
    }

    SECTION("First blocker on diagonal limits farther blockers") {
        ches::Bitboard blockers = to_bitboard(ches::SquareE3, ches::SquareF4, ches::SquareB2);
        ches::Bitboard bb = ches::bishop_attacks(ches::SquareC1, blockers);

        CHECK(ches::has_square(bb, ches::SquareD2));
        CHECK(ches::has_square(bb, ches::SquareE3));
        CHECK(ches::has_square(bb, ches::SquareB2));
        CHECK(!ches::has_square(bb, ches::SquareF4));
        CHECK(!ches::has_square(bb, ches::SquareA3));
    }
}
