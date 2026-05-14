#pragma once

#include "cheslib/move.hpp"

namespace cheslib {

class MoveList {
  public:
    static constexpr size_t MaxSize = 256;

  public:
    constexpr MoveList() : _size(0) {
    }

    constexpr size_t size() const {
        return _size;
    }

    constexpr const Move *begin() const {
        return _moves;
    }

    constexpr const Move *end() const {
        return _moves + _size;
    }

    constexpr Move at(size_t index) const {
        assert(index < _size);
        return _moves[index];
    }

    constexpr void add(Move move) {
        assert(_size < MaxSize);
        _moves[_size] = move;
        ++_size;
    }

    constexpr bool has(Move target) const {
        for (Move move : *this) {
            if (move == target) {
                return true;
            }
        }
        return false;
    }

    /// @return `Move::none()` if not found
    constexpr Move find(Square from, Square to) const {
        constexpr uint16_t mask = 0xFFFu;
        const uint16_t target = from | (to << 6);

        for (Move move : *this) {
            if ((move.data() & mask) == target) {
                return move;
            }
        }

        return Move::none();
    }

  private:
    Move _moves[MaxSize];
    size_t _size;
};

} // namespace cheslib
