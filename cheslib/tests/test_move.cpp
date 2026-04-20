#include <catch2/catch_test_macros.hpp>

#include "cheslib/move.hpp"
#include "cheslib/types.hpp"

using namespace cheslib;

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

TEST_CASE("Move: Move::promo_piece() decodes promotion flags", "[move]") {
    CHECK(Move(SquareA7, SquareA8, KnightPromo).promo_piece() == Knight);
    CHECK(Move(SquareA7, SquareA8, BishopPromo).promo_piece() == Bishop);
    CHECK(Move(SquareA7, SquareA8, RookPromo).promo_piece() == Rook);
    CHECK(Move(SquareA7, SquareA8, QueenPromo).promo_piece() == Queen);

    CHECK(Move(SquareA7, SquareB8, KnightPromoCap).promo_piece() == Knight);
    CHECK(Move(SquareA7, SquareB8, BishopPromoCap).promo_piece() == Bishop);
    CHECK(Move(SquareA7, SquareB8, RookPromoCap).promo_piece() == Rook);
    CHECK(Move(SquareA7, SquareB8, QueenPromoCap).promo_piece() == Queen);
}

TEST_CASE("Move: Equality", "[move]") {
    const Move a(SquareE2, SquareE4, DoublePawnPush);
    const Move b(SquareE2, SquareE4, DoublePawnPush);
    const Move c(SquareE2, SquareE4, QuietMove);

    CHECK(a == b);
    CHECK_FALSE(a == c);
}

TEST_CASE("Move: constexpr", "[move]") {
    constexpr Move m(SquareA7, SquareB8, QueenPromoCap);

    STATIC_CHECK(m.from() == SquareA7);
    STATIC_CHECK(m.to() == SquareB8);
    STATIC_CHECK(m.flag() == QueenPromoCap);
    STATIC_CHECK(m.promo_piece() == Queen);
    STATIC_CHECK(m.is_promotion());
    STATIC_CHECK(m.is_capture());
    STATIC_CHECK_FALSE(m == Move{});
}

TEST_CASE("MoveList: Check functionality", "[move_list]") {
    MoveList moves{};

    const Move m1(SquareE2, SquareE4, DoublePawnPush);
    const Move m2(SquareG1, SquareF3, QuietMove);
    const Move m3(SquareD1, SquareD8, Capture);

    moves.add(SquareE2, SquareE4, DoublePawnPush);
    moves.add(SquareG1, SquareF3, QuietMove);
    moves.add(SquareD1, SquareD8, Capture);

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

TEST_CASE("MoveList: constexpr", "[move_list]") {
    constexpr MoveList moves = []() {
        MoveList ml{};
        ml.add(SquareE2, SquareE4, DoublePawnPush);
        ml.add(SquareG1, SquareF3, QuietMove);
        ml.add(SquareD1, SquareD8, Capture);
        return ml;
    }();

    constexpr Move move_0(SquareE2, SquareE4, DoublePawnPush);
    constexpr Move move_1(SquareG1, SquareF3, QuietMove);
    constexpr Move move_2(SquareD1, SquareD8, Capture);

    STATIC_CHECK(moves.size() == 3);
    STATIC_CHECK(moves.end() - moves.begin() == moves.size());
    STATIC_CHECK(moves[0] == move_0);
    STATIC_CHECK(moves[1] == move_1);
    STATIC_CHECK(moves[2] == move_2);

    STATIC_CHECK(moves.has(move_0));
    STATIC_CHECK(moves.has(move_1));
    STATIC_CHECK(moves.has(move_2));
    STATIC_CHECK_FALSE(moves.has(Move(SquareA2, SquareA3, QuietMove)));
}
