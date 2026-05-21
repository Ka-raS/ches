#pragma once

#include <cassert>
#include <cstddef>
#include <utility>

namespace cheslib {

/// std::array with push pop shenanigans
template <typename T, size_t N>
class Array {
  public:
    constexpr Array() = default;

    constexpr size_t size() const {
        return _size;
    }

    constexpr void resize(size_t size) {
        assert(size <= N);
        _size = size;
    }

    template <typename... Args>
    constexpr void push(Args &&...args) {
        assert(_size < N);
        _data[_size] = T{std::forward<Args>(args)...};
        ++_size;
    }

    constexpr T pop() {
        assert(_size > 0);
        T value = std::move(_data[_size - 1]);
        --_size;
        return value;
    }

    constexpr T &operator[](size_t index) {
        assert(index < _size);
        return _data[index];
    }
    constexpr const T &operator[](size_t index) const {
        assert(index < _size);
        return _data[index];
    }

    constexpr T *begin() {
        return _data;
    }

    constexpr const T *begin() const {
        return _data;
    }

    constexpr T *end() {
        return _data + _size;
    }

    constexpr const T *end() const {
        return _data + _size;
    }

  private:
    T _data[N];
    size_t _size = 0;
};

} // namespace cheslib
