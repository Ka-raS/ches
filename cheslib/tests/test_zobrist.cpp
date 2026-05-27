#include <unordered_set>

#include <catch2/catch_test_macros.hpp>

#include "zobrist.hpp"

using namespace cheslib;

TEST_CASE("Zobrist: thats some good keys", "[zobrist]") {
    std::unordered_set<ZobristKey> keys;
    keys.reserve(800);

    auto insert = [&keys](ZobristKey key) -> void {
        bool no_collision = keys.insert(key).second;
        CHECK(key != 0);
        CHECK(no_collision);
    };

    insert(zobrist::side());

    for (CastleFlag flag = NoCastles; flag <= BothCastles; flag = CastleFlag(flag + 1)) {
        insert(zobrist::castling(flag));
    }

    CHECK(zobrist::en_passant(FileCNT) == 0);
    for (File file = FileA; file <= FileH; ++file) {
        insert(zobrist::en_passant(file));
    }

    for (Piece piece = Piece(0); piece < PieceCNT; ++piece) {
        for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
            insert(zobrist::piece(piece, sq));
        }
    }

    SECTION("double xor sum are unique") {
        std::unordered_set<ZobristKey> xor_sums;
        xor_sums.reserve(800 * 800);

        for (auto one = keys.begin(); one != keys.end(); ++one) {
            for (auto two = std::next(one); two != keys.end(); ++two) {
                ZobristKey xor_sum = *one ^ *two;
                bool no_collision = xor_sums.insert(xor_sum).second;
                CHECK(xor_sum != 0);
                CHECK(no_collision);
            }
        }
    }
}
