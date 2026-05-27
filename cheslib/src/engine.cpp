#include <stdexcept>

#include "cheslib/engine.hpp"

#include "movegen.hpp"
#include "negamax.hpp"

namespace cheslib {

struct Engine::PositionImpl {
    Position d;
};

struct Engine::NegamaxImpl {
    Negamax d;
};

Engine::Engine(unsigned search_depth, int threads_requested)
    : _position(*new (_buffer) PositionImpl(Position::initial())),
      _negamax(*new (_buffer + sizeof(PositionImpl)) NegamaxImpl({search_depth, threads_requested})),
      _legal_moves(movegen::legals(_position.d)) {

    static_assert(sizeof(_buffer) >= sizeof(PositionImpl) + sizeof(NegamaxImpl));
    static_assert(BufferAlign >= std::max(alignof(PositionImpl), alignof(NegamaxImpl)));
}

Engine::~Engine() {
    _negamax.~NegamaxImpl();
    _position.~PositionImpl();
}

bool Engine::is_game_over() const {
    return _legal_moves.size() == 0 || _position.d.is_50move_draw() || _position.d.is_3fold_repetition();
}

void Engine::reset_game() {
    _position.d = Position::initial();
    _negamax.d.reset();
}

const std::array<Piece, SquareCNT> &Engine::board() const {
    return _position.d.pieces().board();
}

const Array<Move, 256> &Engine::legal_moves() const {
    return _legal_moves;
}

void Engine::do_move(Move move) {
#ifdef __cpp_exceptions
    if (is_game_over()) {
        throw std::logic_error("game over");
    } else if (std::ranges::find(_legal_moves, move) == _legal_moves.end()) {
        throw std::invalid_argument("illegal move");
    }
#endif

    _position.d.do_move(move);
    _position.d.trim_history();
    _legal_moves = movegen::legals(_position.d);
}

void Engine::start_move_search() {
#ifdef __cpp_exceptions
    if (is_game_over()) {
        throw std::logic_error("game over");
    }
#endif

    _negamax.d.start_search(_position.d, _legal_moves);
}

bool Engine::is_searching() const {
    return _negamax.d.is_searching();
}

Move Engine::search_result() const {
#ifdef __cpp_exceptions
    if (is_searching()) {
        throw std::logic_error("still searching");
    }
#endif

    return _negamax.d.result();
}

} // namespace cheslib
