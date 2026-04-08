#include "zobrist.hpp"

namespace ches {

namespace {

// see: https://en.wikipedia.org/wiki/Xorshift
class XorShift64 {
  public:
    consteval XorShift64(ZKey seed) : _state(seed) {
    }

    consteval ZKey next() {
        ZKey x = _state;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        _state = x;
        return x;
    }

  private:
    ZKey _state;
};

consteval ZKey single(ZKey seed) {
    return XorShift64(seed).next();
}

template <size_t N>
consteval std::array<ZKey, N> array1D(ZKey seed) {
    std::array<ZKey, N> arr{};
    XorShift64 rng(seed);

    for (size_t i = 0; i < N; ++i) {
        arr[i] = rng.next();
    }
    return arr;
}

template <size_t Rows, size_t Cols>
consteval std::array<std::array<ZKey, Cols>, Rows> array2D(ZKey seed) {
    std::array<std::array<ZKey, Cols>, Rows> arr{};
    XorShift64 rng(seed);

    for (size_t i = 0; i < Rows; ++i) {
        for (size_t j = 0; j < Cols; ++j) {
            arr[i][j] = rng.next();
        }
    }
    return arr;
}

} // namespace

namespace detail {

constexpr ZKey side_to_move_key = single(37);
constexpr std::array<ZKey, 4> castling_keys = array1D<4>(42);
constexpr std::array<ZKey, FileCNT> en_passant_keys = array1D<FileCNT>(67);
constexpr std::array<std::array<ZKey, SquareCNT>, PieceCNT> piece_keys = array2D<PieceCNT, SquareCNT>(69);

} // namespace detail

} // namespace ches