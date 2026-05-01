#include "cheslib/engine.hpp"

#include "position.hpp"

namespace cheslib {

Engine::Engine() : _position(*(new (_position_buffer) Position(Position::initial()))) {
    static_assert(sizeof(Position) == sizeof(_position_buffer));
    static_assert(alignof(Position) == 8);
}

Engine::~Engine() {
    _position.~Position();
}

const std::array<Piece, SquareCNT> &Engine::board() const {
    return _position.pieces().board();
}

} // namespace cheslib
