#include <catch2/catch_test_macros.hpp>

#include "cheslib/move_list.hpp"

using namespace ::cheslib;

TEST_CASE("MoveList: Default constructor", "[move_list]") {
    MoveList moves;

    CHECK(moves.size() == 0);
    CHECK(moves.begin() == moves.end());
    CHECK(moves.find(SquareA2, SquareA3) == nullptr);

    Move move(SquareA2, SquareA3, QuietMove);
    CHECK_FALSE(moves.has(move));
}

TEST_CASE("MoveList: add and find", "[move_list]") {
    MoveList moves;

    Move m1(SquareA2, SquareA3, QuietMove);
    Move m2(SquareB2, SquareB4, DoublePawnPush);
    Move m3(SquareC2, SquareC3, QuietMove);

    moves.add(m1);
    moves.add(m2);

    CHECK(moves.size() == 2);
    CHECK(moves.has(m2));
    CHECK_FALSE(moves.has(m3));

    const Move *found = moves.find(SquareA2, SquareA3);
    REQUIRE(found != nullptr);
    CHECK(*found == m1);
}

TEST_CASE("MoveList: iteration", "[move_list]") {
    MoveList moves;

    Move data[] = {
        {SquareA2, SquareA3, QuietMove}, //
        {SquareB2, SquareB4, DoublePawnPush}
    };

    moves.add(data[0]);
    moves.add(data[1]);

    std::size_t i = 0;
    for (Move move : moves) {
        CHECK(move == data[i]);
        ++i;
    }
    CHECK(i == 2);
}
