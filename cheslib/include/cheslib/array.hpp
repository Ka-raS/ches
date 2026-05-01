#pragma once

#include <cassert>
#include <concepts>
#include <cstddef>
#include <memory>

namespace cheslib {

/**
 * stack alloc array with add/pop shenanigans
 */
template <typename T, std::size_t N>
class Array {
  public:
    Array();
    ~Array() = default;
    std::size_t size() const;
    const T *begin() const;
    const T *end() const;
    const T &operator[](std::size_t index) const;
    T pop();

    template <typename... Args>
        requires std::constructible_from<T, Args...>
    void add(Args &&...args);

  private:
    T _data[N];
    std::size_t _size;
};

template <typename T, std::size_t N>
Array<T, N>::Array() : _data{}, _size(0) {
}

template <typename T, std::size_t N>
std::size_t Array<T, N>::size() const {
    return _size;
}

template <typename T, std::size_t N>
const T *Array<T, N>::begin() const {
    return _data;
}

template <typename T, std::size_t N>
const T *Array<T, N>::end() const {
    return _data + _size;
}

template <typename T, std::size_t N>
const T &Array<T, N>::operator[](std::size_t index) const {
    assert(index < _size);
    return _data[index];
}

template <typename T, std::size_t N>
T Array<T, N>::pop() {
    assert(_size > 0);
    --_size;
    return std::move(_data[_size]);
}

template <typename T, std::size_t N>
template <typename... Args>
    requires std::constructible_from<T, Args...>
void Array<T, N>::add(Args &&...args) {
    assert(_size < N);
    std::construct_at(&_data[_size], std::forward<Args>(args)...);
    ++_size;
}

} // namespace cheslib
