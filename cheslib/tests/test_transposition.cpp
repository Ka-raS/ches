#include <catch2/catch_test_macros.hpp>

#include "transposition.hpp"

using namespace cheslib;

TEST_CASE("Transposition: simple store and get", "[transposition]") {
    const ZobristKey key = 0x123456789ABCDEF;
    MoveScore move{Move(SquareE2, SquareE4, DoublePawnPush), int16_t(1234)};
    const Bound bound = Bound::Lower;
    const unsigned depth = 5u;

    std::unique_ptr<TranspositionTable> table = std::make_unique<TranspositionTable>();
    table->store(key, move, bound, depth);

    const Transposition entry = table->get(key);
    REQUIRE(entry.is_match(key));
    CHECK(entry.move() == move.move);
    CHECK(entry.score() == move.score);
    CHECK(entry.bound() == bound);
    CHECK(entry.depth() == depth);
}

TEST_CASE("Transposition: no overwrite with shallower depth", "[transposition]") {
    const ZobristKey key = 0x2222333344445555;
    const Move move1(SquareE2, SquareE4, DoublePawnPush);
    const Move move2(SquareE2, SquareE3, QuietMove);

    std::unique_ptr<TranspositionTable> table = std::make_unique<TranspositionTable>();
    table->store(key, MoveScore{move1, int16_t(10)}, Bound::Lower, 6u);
    table->store(key, MoveScore{move2, int16_t(20)}, Bound::Lower, 4u);

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
    table->store(key, MoveScore{move1, int16_t(1)}, Bound::Upper, 3u);
    table->store(key, MoveScore{move2, int16_t(2)}, Bound::Upper, 8u);

    const Transposition entry = table->get(key);
    REQUIRE(entry.is_match(key));
    CHECK(entry.move() == move2);
    CHECK(entry.score() == 2);
    CHECK(entry.bound() == Bound::Upper);
    CHECK(entry.depth() == 8u);
}

TEST_CASE("Transposition: overwrites with Bound::Exact", "[transposition]") {
    const ZobristKey key = 0xCAFEBABEDEADBEEF;
    const Move move1(SquareB1, SquareC3, QuietMove);
    const Move move2(SquareB1, SquareA3, QuietMove);

    std::unique_ptr<TranspositionTable> table = std::make_unique<TranspositionTable>();
    table->store(key, MoveScore{move1, int16_t(7)}, Bound::Upper, 7u);
    table->store(key, MoveScore{move2, int16_t(8)}, Bound::Exact, 4u);

    const Transposition entry = table->get(key);
    REQUIRE(entry.is_match(key));
    CHECK(entry.score() == 8);
    CHECK(entry.move() == move2);
    CHECK(entry.bound() == Bound::Exact);
    CHECK(entry.depth() == 4u);
}