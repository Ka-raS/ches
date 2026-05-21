#include <catch2/catch_test_macros.hpp>

#include "position_state.hpp"

using namespace cheslib;

TEST_CASE("PositionState: Initial state", "[state]") {
    const PositionState s = PositionState::initial();

    CHECK(s.castle_flag() == BothCastles);
    CHECK(s.en_passant() == FileCNT);
    CHECK(s.side_to_move() == White);
    CHECK(s.rule50_count() == 0);
}

TEST_CASE("PositionState: Constructor encodes, method decodes", "[state]") {
    const CastleFlag flags = CastleFlag(WhiteShortCastles | BlackLongCastles);
    const PositionState s(flags, FileE, Black, 37);

    CHECK(s.castle_flag() == flags);
    CHECK(s.en_passant() == FileE);
    CHECK(s.side_to_move() == Black);
    CHECK(s.rule50_count() == 37);

    CHECK(s.can_castles(WhiteShortCastles));
    CHECK_FALSE(s.can_castles(WhiteLongCastles));
    CHECK_FALSE(s.can_castles(BlackShortCastles));
    CHECK(s.can_castles(BlackLongCastles));
}

TEST_CASE("PositionState: Equality", "[state]") {
    const PositionState a(BothCastles, FileC, Black, 12);
    const PositionState b(BothCastles, FileC, Black, 12);
    const PositionState c(BothCastles, FileC, White, 12);

    CHECK(a == b);
    CHECK_FALSE(a == c);
}

TEST_CASE("PositionState: PositionState::set_castles() changes only castling rights", "[state]") {
    PositionState s(BothCastles, FileG, Black, 44);
    s.revoke_castles(WhiteLongCastles);

    CHECK_FALSE(s.castle_flag() & WhiteLongCastles);
    CHECK(s.en_passant() == FileG);
    CHECK(s.side_to_move() == Black);
    CHECK(s.rule50_count() == 44);
}

TEST_CASE("PositionState: PositionState::set_en_passant() changes only en passant file", "[state]") {
    PositionState s(WhiteCastles, FileCNT, Black, 19);

    s.set_en_passant(FileB);

    CHECK(s.castle_flag() == WhiteCastles);
    CHECK(s.en_passant() == FileB);
    CHECK(s.side_to_move() == Black);
    CHECK(s.rule50_count() == 19);
}

TEST_CASE("PositionState: PositionState::switch_side() changes only side", "[state]") {
    PositionState s(BlackCastles, FileD, White, 8);

    s.switch_side();
    CHECK(s.side_to_move() == Black);
    CHECK(s.castle_flag() == BlackCastles);
    CHECK(s.en_passant() == FileD);
    CHECK(s.rule50_count() == 8);

    s.switch_side();
    CHECK(s.side_to_move() == White);
    CHECK(s.castle_flag() == BlackCastles);
    CHECK(s.en_passant() == FileD);
    CHECK(s.rule50_count() == 8);
}

TEST_CASE("PositionState: PositionState::increment_rule50() increases counter only", "[state]") {
    PositionState s(WhiteCastles, FileA, Black, 55);

    s.increment_rule50();

    CHECK(s.rule50_count() == 56);
    CHECK(s.castle_flag() == WhiteCastles);
    CHECK(s.en_passant() == FileA);
    CHECK(s.side_to_move() == Black);
}

TEST_CASE("PositionState: PositionState::reset_rule50() clears counter only", "[state]") {
    PositionState s(BlackShortCastles, FileH, Black, 77);

    s.reset_rule50();

    CHECK(s.rule50_count() == 0);
    CHECK(s.castle_flag() == BlackShortCastles);
    CHECK(s.en_passant() == FileH);
    CHECK(s.side_to_move() == Black);
}

TEST_CASE("PositionState: Doing random stuffs idk", "[state]") {
    PositionState s(BothCastles, FileCNT, Black, 3);

    s.reset_rule50();
    s.switch_side();
    s.set_en_passant(FileE);
    s.revoke_castles(WhiteCastles);

    CHECK(s.rule50_count() == 0);
    CHECK(s.side_to_move() == White);
    CHECK(s.en_passant() == FileE);
    CHECK_FALSE(s.castle_flag() & WhiteCastles);
}