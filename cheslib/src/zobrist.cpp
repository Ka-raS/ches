#include <cstddef>

#include "zobrist.hpp"

namespace cheslib::zobrist {

namespace {

// see: https://en.wikipedia.org/wiki/Xorshift
class XorShift64 {
  public:
    consteval XorShift64(uint64_t seed) : _x(seed) {
    }

    consteval uint64_t next() {
        _x ^= _x << 13;
        _x ^= _x >> 7;
        _x ^= _x << 17;
        return _x;
    }

  private:
    uint64_t _x;
};

template <size_t N>
consteval std::array<ZKey, N> rng(uint64_t seed, size_t discard = N) {
    std::array<ZKey, N> arr{};
    XorShift64 rng(seed);

    for (size_t i = 0; i < N; ++i) {
        arr[i] = (i == discard) ? 0 : rng.next();
    }
    return arr;
}

} // namespace

namespace detail {

constexpr ZKey side_key = XorShift64(0xCAFEBABEDEADBEEF).next();

constexpr std::array<ZKey, BothCastles + 1> castling_keys = rng<BothCastles + 1>(side_key);

constexpr std::array<ZKey, FileCNT + 1> en_passant_keys = rng<FileCNT + 1>(castling_keys.back(), FileCNT);

constexpr std::array<ZKey, (int)PieceCNT * SquareCNT> piece_keys =
    rng<(int)PieceCNT * SquareCNT>(en_passant_keys[FileH]);

} // namespace detail

ZKey hash(const std::array<Piece, SquareCNT> &board, const State state) {
    ZKey key = 0;

    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        Piece piece = board[sq];
        if (piece < PieceCNT) {
            key ^= detail::piece_keys[piece * (int)SquareCNT + sq];
        }
    }

    if (state.side_to_move() == Black) {
        key ^= detail::side_key;
    }

    key ^= detail::castling_keys[state.castle_flag()];
    key ^= detail::en_passant_keys[state.en_passant()];

    return key;
}

} // namespace cheslib::zobrist