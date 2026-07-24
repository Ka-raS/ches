#include "transposition.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace ::cheslib;

TEST_CASE("Transposition: simple store and get", "[transposition]") {
    const ZobristKey key = 0x123456789ABCDEF;
    const Move move{SquareE2, SquareE4, DoublePawnPush};
    const Score score = 1234;
    const Bound bound = Bound::Lower;
    const unsigned depth = 5u;

    std::unique_ptr<TranspositionTable> table = std::make_unique<TranspositionTable>();
    table->store(key, move, score, bound, depth);

    const Transposition entry = table->get(key);
    REQUIRE(entry.is_match(key));
    CHECK(entry.move() == move);
    CHECK(entry.score() == score);
    CHECK(entry.bound() == bound);
    CHECK(entry.depth() == depth);
}

TEST_CASE("Transposition: no overwrite with shallower depth", "[transposition]") {
    const ZobristKey key = 0x2222333344445555;
    const Move move1(SquareE2, SquareE4, DoublePawnPush);
    const Move move2(SquareE2, SquareE3, QuietMove);

    std::unique_ptr<TranspositionTable> table = std::make_unique<TranspositionTable>();
    table->store(key, move1, 10, Bound::Lower, 6u);
    table->store(key, move2, 20, Bound::Lower, 4u);

    const Transposition entry = table->get(key);
    REQUIRE(entry.is_match(key));
    CHECK(entry.move() == move1);
    CHECK(entry.score() == 10);
    CHECK(entry.bound() == Bound::Lower);
    CHECK(entry.depth() == 6u);
}

TEST_CASE("Transposition: overwrites with deeper depth", "[transposition]") {
    const ZobristKey key = 0x9876543210FEDCBA;
    const Move move1(SquareA2, SquareA3, QuietMove);
    const Move move2(SquareA2, SquareA4, DoublePawnPush);

    std::unique_ptr<TranspositionTable> table = std::make_unique<TranspositionTable>();
    table->store(key, move1, 1, Bound::Upper, 3u);
    table->store(key, move2, 2, Bound::Upper, 8u);

    const Transposition entry = table->get(key);
    REQUIRE(entry.is_match(key));
    CHECK(entry.move() == move2);
    CHECK(entry.score() == 2);
    CHECK(entry.bound() == Bound::Upper);
    CHECK(entry.depth() == 8u);
}
