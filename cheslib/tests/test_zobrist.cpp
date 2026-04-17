#include <unordered_set>

#include <catch2/catch_test_macros.hpp>

#include "cheslib/types.hpp"

#include "pieces.hpp"
#include "zobrist.hpp"

using namespace cheslib;

TEST_CASE("Zobrist: Zobrist key no collisions", "[zobrist]") {
    std::unordered_set<ZKey> keys;
    keys.reserve(800);

    SECTION("Side key") {
        CHECK(zobrist::side() != 0);
        keys.insert(zobrist::side());
    }

    SECTION("Castling keys") {
        for (CastleFlag flag = NoCastles; flag <= BothCastles; flag = CastleFlag(flag + 1)) {
            ZKey key = zobrist::castling(flag);
            bool no_collision = keys.insert(key).second;

            CAPTURE(flag);
            CHECK(no_collision);
            CHECK(key != 0);
        }
    }

    SECTION("En passant keys") {
        for (File file = FileA; file <= FileH; ++file) {
            ZKey key = zobrist::en_passant(file);
            bool no_collision = keys.insert(key).second;

            CAPTURE(file);
            CHECK(no_collision);
            CHECK(key != 0);
        }

        CHECK(zobrist::en_passant(FileCNT) == 0);
    }

    SECTION("Piece keys") {
        for (Piece piece = Piece(0); piece < PieceCNT; ++piece) {
            for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
                ZKey key = zobrist::piece(piece, sq);
                bool no_collision = keys.insert(key).second;

                CAPTURE(piece, sq);
                CHECK(no_collision);
                CHECK(key != 0);
            }
        }
    }
}