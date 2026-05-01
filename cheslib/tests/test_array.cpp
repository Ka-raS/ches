#include <catch2/catch_test_macros.hpp>

#include "cheslib/array.hpp"
#include "cheslib/move.hpp"

using namespace ::cheslib;

TEST_CASE("Array: Check stuffs", "[array]") {
    Array<Move, 3> moves{};

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

    auto has_move = [&moves](const Move &m) {
        for (Move move : moves) {
            if (move == m) {
                return true;
            }
        }
        return false;
    };

    CHECK(has_move(m1));
    CHECK(has_move(m2));
    CHECK(has_move(m3));
    CHECK_FALSE(has_move({SquareA2, SquareA3, QuietMove}));
}
