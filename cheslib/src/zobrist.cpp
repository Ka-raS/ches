#include "zobrist.hpp"

namespace cheslib {

namespace {

// see: https://en.wikipedia.org/wiki/Xorshift
class XorShift64 {
  public:
    consteval XorShift64(ZKey seed) : _x(seed) {
    }

    consteval ZKey next() {
        _x ^= _x << 13;
        _x ^= _x >> 7;
        _x ^= _x << 17;
        return _x;
    }

  private:
    ZKey _x;
};

template <size_t N>
consteval std::array<ZKey, N> rng(ZKey seed, size_t discard = N) {
    std::array<ZKey, N> arr{};
    XorShift64 rng(seed);

    for (size_t i = 0; i < N; ++i) {
        arr[i] = (i == discard) ? 0 : rng.next();
    }
    return arr;
}

} // namespace

namespace detail {

constexpr ZKey zobrist_side_key = XorShift64(0xCAFEBABEDEADBEEF).next();

constexpr std::array<ZKey, BothCastles + 1> zobrist_castling_keys = rng<BothCastles + 1>(zobrist_side_key);

constexpr std::array<ZKey, FileCNT + 1> zobrist_en_passant_keys =
    rng<FileCNT + 1>(zobrist_castling_keys.back(), FileCNT);

constexpr std::array<ZKey, (int)PieceCNT * SquareCNT> zobrist_piece_keys =
    rng<(int)PieceCNT * SquareCNT>(zobrist_en_passant_keys[FileCNT - 1]);

} // namespace detail

ZKey zobrist_hash(const std::array<Piece, SquareCNT> &board, const State &state) {
    ZKey key = 0;

    for (Square sq = SquareA1; sq < SquareCNT; ++sq) {
        Piece piece = board[sq];
        if (piece < PieceCNT) { // only if square occupied
            key ^= detail::zobrist_piece_keys[piece * (int)SquareCNT + sq];
        }
    }

    if (state.side_to_move() == Black) {
        key ^= detail::zobrist_side_key;
    }

    key ^= detail::zobrist_castling_keys[state.castle_flag()];
    key ^= detail::zobrist_en_passant_keys[state.en_passant()];

    return key;
}

} // namespace ches