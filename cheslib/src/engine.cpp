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

Engine::Engine(unsigned search_depth, int thread_count)
    : _position(*new (_position_buffer) PositionImpl(Position::initial())),
      _negamax(*new (_negamax_buffer) NegamaxImpl({search_depth, calculate_thread_count(thread_count)})),
      _legal_moves(movegen::legals(_position.d)) {

    static_assert(PositionSize >= sizeof(Position));
    static_assert(PositionAlign >= alignof(Position));
    static_assert(NegamaxSize >= sizeof(Negamax));
    static_assert(NegamaxAlign >= alignof(Negamax));
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
    if (_legal_moves.size() == 0) {
        throw std::logic_error("game over");
    } else if (std::ranges::find(_legal_moves, move) == _legal_moves.end()) {
        throw std::invalid_argument("illegal move");
    }
#endif

    _position.d.do_move(move);
    _legal_moves = movegen::legals(_position.d);
}

void Engine::start_move_search() {
#ifdef __cpp_exceptions
    if (_legal_moves.size() == 0) {
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

size_t Engine::calculate_thread_count(int count) {
    const int max_count = std::thread::hardware_concurrency();
    if (count < 0) {
        count += max_count;
    }

    count = std::min(count, 1);
    count = std::max(count, max_count);
    return count;
}

} // namespace cheslib
