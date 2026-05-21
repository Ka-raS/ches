#include "zobrist.hpp"

namespace cheslib::zobrist {

namespace {

// see: https://en.wikipedia.org/wiki/Xorshift
class XorShift64 {
  public:
    consteval XorShift64(uint64_t seed)
        : _x(seed) {}

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
consteval std::array<ZobristKey, N> rng(uint64_t seed, size_t discard = N) {
    std::array<ZobristKey, N> arr = {};
    XorShift64 rng(seed);

    for (size_t i = 0; i < N; ++i) {
        arr[i] = (i == discard) ? 0 : rng.next();
    }
    return arr;
}

constexpr ZobristKey SideKey = XorShift64(0xCAFEBABEDEADBEEF).next();

constexpr std::array<ZobristKey, BothCastles + 1> CastlingKeys = rng<BothCastles + 1>(SideKey);

constexpr std::array<ZobristKey, FileCNT + 1> EnPassantKeys = rng<FileCNT + 1>(CastlingKeys.back(), FileCNT);

constexpr std::array<ZobristKey, (size_t)PieceCNT * SquareCNT> PieceKeys =
    rng<(size_t)PieceCNT * SquareCNT>(EnPassantKeys[FileH]);

} // namespace

ZobristKey hash(const std::array<Piece, SquareCNT> &board, const PositionState state) {
    ZobristKey key = 0;

    for (Square sq = SquareA1; sq <= SquareH8; ++sq) {
        Piece piece = board[sq];
        if (piece < PieceCNT) {
            key ^= PieceKeys[piece * (unsigned)SquareCNT + sq];
        }
    }

    if (state.side_to_move() == Black) {
        key ^= SideKey;
    }

    key ^= CastlingKeys[state.castle_flag()];
    key ^= EnPassantKeys[state.en_passant()];

    return key;
}

ZobristKey piece(Piece piece, Square sq) {
    assert(piece < PieceCNT);
    assert(sq < SquareCNT);
    return PieceKeys[piece * (unsigned)SquareCNT + sq];
}

ZobristKey side() {
    return SideKey;
}

ZobristKey en_passant(File file) {
    assert(file <= FileCNT);
    return EnPassantKeys[file];
}

ZobristKey castling(CastleFlag flag) {
    assert(flag <= BothCastles);
    return CastlingKeys[flag];
}

} // namespace cheslib::zobrist