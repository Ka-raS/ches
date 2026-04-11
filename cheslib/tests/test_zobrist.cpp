#include <unordered_set>

#include <catch2/catch_test_macros.hpp>

#include "cheslib/types.hpp"

#include "piece_bitboards.hpp"
#include "zobrist.hpp"

using namespace ches;

TEST_CASE("Zobrist: Zobrist key no collisions", "[zobrist]") {
    std::unordered_set<ZKey> keys;
    keys.reserve(
        detail::zobrist_piece_keys.size() + detail::zobrist_castling_keys.size() +
        detail::zobrist_en_passant_keys.size() + 1
    );

    SECTION("Side key") {
        CHECK(zobrist_side() != 0);
        keys.insert(zobrist_side());
    }

    SECTION("Castling keys") {
        for (CastleFlag flag = NoCastles; flag <= BothCastles; flag = CastleFlag(flag + 1)) {
            ZKey key = zobrist_castling(flag);
            bool no_collision = keys.insert(key).second;

            CAPTURE(flag);
            CHECK(no_collision);
            CHECK(key != 0);
        }
    }

    SECTION("En passant keys") {
        for (File file = FileA; file < FileCNT; ++file) {
            ZKey key = zobrist_en_passant(file);
            bool no_collision = keys.insert(key).second;

            CAPTURE(file);
            CHECK(no_collision);
            CHECK(key != 0);
        }

        CHECK(zobrist_en_passant(FileCNT) == 0);
    }

    SECTION("Piece keys") {
        for (Piece piece = WhitePawn; piece < PieceCNT; ++piece) {
            for (Square sq = SquareA1; sq < SquareCNT; ++sq) {
                ZKey key = zobrist_piece(piece, sq);
                bool no_collision = keys.insert(key).second;

                CAPTURE(piece, sq);
                CHECK(no_collision);
                CHECK(key != 0);
            }
        }
    }
}