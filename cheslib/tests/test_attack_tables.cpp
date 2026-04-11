#include <array>

#include <catch2/catch_test_macros.hpp>

#include "cheslib/types.hpp"

#include "attack_tables.hpp"
#include "utils.hpp"

using namespace ches;

TEST_CASE("Attack tables: Check pieces stepping attack tables", "[attack_tables]") {
    SECTION("Knight") {
        // corners
        CHECK(knight_attacks(SquareA1) == to_bitboard(SquareB3, SquareC2));
        CHECK(knight_attacks(SquareD1) == to_bitboard(SquareB2, SquareC3, SquareE3, SquareF2));

        // center
        CHECK(std::popcount(knight_attacks(SquareE4)) == 8);
    }

    SECTION("King") {
        // corner
        CHECK(king_attacks(SquareH8) == to_bitboard(SquareG8, SquareG7, SquareH7));

        // center
        Bitboard expected = to_bitboard(SquareC3, SquareC4, SquareC5, SquareD3, SquareD5, SquareE3, SquareE4, SquareE5);
        CHECK(king_attacks(SquareD4) == expected);
    }

    SECTION("Pawn") {
        // edge files
        CHECK(pawn_attacks<White>(SquareA2) == to_bitboard(SquareB3));
        CHECK(pawn_attacks<Black>(SquareH7) == to_bitboard(SquareG6));

        // center
        CHECK(pawn_attacks<White>(SquareE4) == to_bitboard(SquareD5, SquareF5));
        CHECK(pawn_attacks<Black>(SquareE5) == to_bitboard(SquareD4, SquareF4));

        // back ranks
        CHECK(pawn_attacks<White>(SquareA8) == 0);
        CHECK(pawn_attacks<Black>(SquareA1) == 0);
    }
}

TEST_CASE("Attack tables: Check rook sliding attack magic table", "[attack_tables]") {
    SECTION("Empty board") {
        Bitboard attacks = rook_attacks(SquareD4, 0);
        CHECK(std::popcount(attacks) == 14); // 14 attack squares
        CHECK(has_square(attacks, SquareD1));
        CHECK(has_square(attacks, SquareD8));
        CHECK(has_square(attacks, SquareA4));
        CHECK(has_square(attacks, SquareH4));
    }

    SECTION("Corner on empty board") {
        Bitboard attacks = rook_attacks(SquareA1, 0);
        CHECK(std::popcount(attacks) == 14);
        CHECK(has_square(attacks, SquareA8));
        CHECK(has_square(attacks, SquareH1));
    }

    SECTION("Blockers") {
        Bitboard blockers = to_bitboard(SquareD6, SquareB4);
        Bitboard bb = rook_attacks(SquareD4, blockers);

        // upward stops at D6
        CHECK(has_square(bb, SquareD5));
        CHECK(has_square(bb, SquareD6));
        CHECK(!has_square(bb, SquareD7));

        // left stops at B4
        CHECK(has_square(bb, SquareC4));
        CHECK(has_square(bb, SquareB4));
        CHECK(!has_square(bb, SquareA4));

        // edge squares
        CHECK(has_square(bb, SquareH4));
        CHECK(has_square(bb, SquareD1));
    }

    SECTION("Immediate orthogonal blockers") {
        Bitboard blockers = to_bitboard(SquareD5, SquareD3, SquareC4, SquareE4);
        Bitboard bb = rook_attacks(SquareD4, blockers);
        CHECK(bb == to_bitboard(SquareD5, SquareD3, SquareC4, SquareE4));
    }

    SECTION("First blocker on ray limits farther blockers") {
        Bitboard blockers = to_bitboard(SquareD6, SquareD7, SquareB4);
        Bitboard bb = rook_attacks(SquareD4, blockers);

        CHECK(has_square(bb, SquareD5));
        CHECK(has_square(bb, SquareD6));
        CHECK(!has_square(bb, SquareD7));
        CHECK(!has_square(bb, SquareD8));

        CHECK(has_square(bb, SquareC4));
        CHECK(has_square(bb, SquareB4));
        CHECK(!has_square(bb, SquareA4));
    }
}

TEST_CASE("Attack tables: Check bishop sliding attack magic table", "[attack_tables]") {
    SECTION("Empty board") {
        Bitboard bb = bishop_attacks(SquareD4, 0);
        CHECK(std::popcount(bb) == 13);
        CHECK(has_square(bb, SquareA1));
        CHECK(has_square(bb, SquareG7));
    }

    SECTION("Corner on empty board") {
        Bitboard bb = bishop_attacks(SquareH1, 0);
        CHECK(std::popcount(bb) == 7);
        CHECK(has_square(bb, SquareG2));
        CHECK(has_square(bb, SquareA8));
    }

    SECTION("Blockers") {
        Bitboard blockers = 1ULL << SquareB2;
        Bitboard bb = bishop_attacks(SquareA1, blockers);
        CHECK(has_square(bb, SquareB2));
        CHECK(!has_square(bb, SquareA1));
        CHECK(!has_square(bb, SquareB3));

        // F6 surrounded
        blockers = to_bitboard(SquareE5, SquareE7, SquareG5, SquareG7);
        bb = bishop_attacks(SquareF6, blockers);
        for (Square sq : {SquareD4, SquareD8, SquareH4, SquareH8}) {
            CHECK(!has_square(bb, sq));
        }
    }

    SECTION("Immediate diagonal blockers") {
        Bitboard blockers = to_bitboard(SquareC5, SquareE5, SquareC3, SquareE3);
        Bitboard bb = bishop_attacks(SquareD4, blockers);
        CHECK(bb == to_bitboard(SquareC5, SquareE5, SquareC3, SquareE3));
    }

    SECTION("First blocker on diagonal limits farther blockers") {
        Bitboard blockers = to_bitboard(SquareE3, SquareF4, SquareB2);
        Bitboard bb = bishop_attacks(SquareC1, blockers);

        CHECK(has_square(bb, SquareD2));
        CHECK(has_square(bb, SquareE3));
        CHECK(has_square(bb, SquareB2));
        CHECK(!has_square(bb, SquareF4));
        CHECK(!has_square(bb, SquareA3));
    }
}
