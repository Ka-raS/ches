#include "cheslib/engine.hpp"

#include "movegen.hpp"

namespace cheslib {

struct Engine::PositionImpl {
    Position d;
};

Engine::PositionImpl &Engine::construct_position(std::byte *buffer) {
    static_assert(PositionSize == sizeof(Position));
    static_assert(PositionAlign == alignof(Position));

    PositionImpl *ptr = new (buffer) PositionImpl{Position::initial()};
    return *ptr;
}

Engine::Engine() : _position(construct_position(_position_buffer)) {
}

Engine::~Engine() {
    _position.~PositionImpl();
}

const std::array<Piece, SquareCNT> &Engine::board() const {
    return _position.d.pieces().board();
}
bool Engine::try_do_move(Move move) {
    return _position.d.try_do_pseudo(move);
}

MoveList Engine::generate_legal_moves() const {
    return movegen::legals(_position.d);
}

} // namespace cheslib
