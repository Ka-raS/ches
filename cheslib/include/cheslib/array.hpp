#pragma once

#include <cassert>
#include <cstddef>
#include <utility>

namespace cheslib {

/// stack based std::vector
template <typename T, size_t N>
    requires std::is_trivial_v<T>
class Array {
  public:
    constexpr size_t size() const {
        return _size;
    }

    constexpr void resize(size_t size) {
        assert(size <= N);
        _size = size;
    }

    template <typename... Args>
        requires std::constructible_from<T, Args...>
    constexpr void push(Args &&...args) {
        assert(_size < N);
        _data[_size] = T{std::forward<Args>(args)...};
        ++_size;
    }

    constexpr T pop() {
        assert(_size > 0);
        --_size;
        return _data[_size];
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
