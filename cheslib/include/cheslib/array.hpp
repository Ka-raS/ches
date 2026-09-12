#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <utility>

namespace cheslib {

/// basically `std::inplace_vector`
template <typename T, size_t N>
    requires std::is_trivially_default_constructible_v<T> && std::is_trivially_copyable_v<T>
class Array {
  public:
    constexpr Array() = default;

    constexpr size_t size() const {
        return _size;
    }

    constexpr void clear() {
        _size = 0;
    }

    constexpr const T &back() const {
        assert(_size > 0);
        return _data[_size - 1];
    }

    constexpr void pop_back() {
        assert(_size > 0);
        --_size;
    }

    template <typename... Args>
        requires std::is_constructible_v<T, Args...>
    constexpr void emplace_back(Args &&...args) {
        assert(_size < N);
        _data[_size] = T{std::forward<Args>(args)...};
        ++_size;
    }

    template <typename InputIt>
        requires std::indirectly_copyable<InputIt, T *>
    constexpr void assign(InputIt begin, InputIt end) {
        assert(std::distance(begin, end) <= N);
        std::copy(begin, end, _data);
        _size = std::distance(begin, end);
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
