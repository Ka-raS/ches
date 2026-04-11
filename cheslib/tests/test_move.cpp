#include <catch2/catch_test_macros.hpp>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

using namespace ches;

TEST_CASE("Move: Contructor encodes, methods decode", "[move]") {
    SECTION("Quiet move") {
        const Move quiet(SquareB1, SquareC3, QuietMove);
        CHECK(quiet.from() == SquareB1);
        CHECK(quiet.to() == SquareC3);
        CHECK(quiet.flag() == QuietMove);
        CHECK_FALSE(quiet.is_capture());
        CHECK_FALSE(quiet.is_promotion());
    }

    SECTION("Capture move") {
        const Move capture(SquareH8, SquareA8, Capture);
        CHECK(capture.from() == SquareH8);
        CHECK(capture.to() == SquareA8);
        CHECK(capture.flag() == Capture);
        CHECK(capture.is_capture());
        CHECK_FALSE(capture.is_promotion());
    }

    SECTION("En passant") {
        const Move en_passant(SquareE5, SquareD6, EnPassant);
        CHECK(en_passant.from() == SquareE5);
        CHECK(en_passant.to() == SquareD6);
        CHECK(en_passant.flag() == EnPassant);
    }

    SECTION("Promotion") {
        const Move promo(SquareA7, SquareA8, QueenPromo);
        CHECK_FALSE(promo.is_capture());
        CHECK(promo.is_promotion());
    }

    SECTION("Promotion capture") {
        const Move promo_capture(SquareA7, SquareB8, QueenPromoCap);
        CHECK(promo_capture.is_capture());
        CHECK(promo_capture.is_promotion());
    }
}

TEST_CASE("Move: Move::promotion_piece() decodes promotion flags", "[move]") {
    CHECK(Move(SquareA7, SquareA8, KnightPromo).promotion_piece() == Knight);
    CHECK(Move(SquareA7, SquareA8, BishopPromo).promotion_piece() == Bishop);
    CHECK(Move(SquareA7, SquareA8, RookPromo).promotion_piece() == Rook);
    CHECK(Move(SquareA7, SquareA8, QueenPromo).promotion_piece() == Queen);

    CHECK(Move(SquareA7, SquareB8, KnightPromoCap).promotion_piece() == Knight);
    CHECK(Move(SquareA7, SquareB8, BishopPromoCap).promotion_piece() == Bishop);
    CHECK(Move(SquareA7, SquareB8, RookPromoCap).promotion_piece() == Rook);
    CHECK(Move(SquareA7, SquareB8, QueenPromoCap).promotion_piece() == Queen);
}

TEST_CASE("Move: Equality", "[move]") {
    const Move a(SquareE2, SquareE4, DoublePawnPush);
    const Move b(SquareE2, SquareE4, DoublePawnPush);
    const Move c(SquareE2, SquareE4, QuietMove);

    CHECK(a == b);
    CHECK_FALSE(a == c);
}

TEST_CASE("MoveList: Check functionality", "[move_list]") {
    MoveList moves{};

    const Move m1(SquareE2, SquareE4, DoublePawnPush);
    const Move m2(SquareG1, SquareF3, QuietMove);
    const Move m3(SquareD1, SquareD8, Capture);

    moves.add(SquareE2, SquareE4, DoublePawnPush);
    moves.add(SquareG1, SquareF3, QuietMove);
    moves.add(SquareD1, SquareD8, Capture);

    CHECK_FALSE(moves.empty());
    CHECK(moves.size() == 3);

    const Move *it = moves.begin();
    CHECK(*it == m1);
    ++it;
    CHECK(*it == m2);
    ++it;
    CHECK(*it == m3);

    CHECK(moves.has(m1));
    CHECK(moves.has(m2));
    CHECK(moves.has(m3));
    CHECK_FALSE(moves.has(Move(SquareA2, SquareA3, QuietMove)));
}
