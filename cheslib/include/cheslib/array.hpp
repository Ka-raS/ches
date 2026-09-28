#pragma once

#include <cassert>
#include <cstddef>
#include <cstring>
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

    template <typename It>
        requires std::same_as<T, std::iter_value_t<It>> && std::contiguous_iterator<It>
    constexpr void assign(const It begin, const It end) {
        _size = std::distance(begin, end);
        assert(_size <= N);
        std::memcpy(_data, std::to_address(begin), _size * sizeof(T));
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
