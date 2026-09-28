#include "cheslib/engine.hpp"
#include "movegen.hpp"
#include "negamax.hpp"

#include <stdexcept>

namespace cheslib {

struct Engine::Impl {
    Position position;
    Negamax negamax;
    std::vector<MoveEntry> history; ///< stores trimmed `MoveEntry` from `Position::_history`
};

Engine::Engine(const unsigned search_depth, const int thread_count) {
    static_assert(sizeof(_buffer) >= sizeof(Impl));
    static_assert(BufferAlign == alignof(Impl));

    Impl *const impl = new (_buffer) Impl{
        .position{Position::initial()}, //
        .negamax{search_depth, thread_count},
        .history{}
    };
    impl->history.reserve(512);
    _legal_moves = movegen::legals(impl->position);
}

Engine::~Engine() {
    Impl *const impl = pimpl();
    impl->negamax.stop_search();
    impl->~Impl();
}

Engine::Impl *Engine::pimpl() {
    return std::launder(reinterpret_cast<Impl *>(_buffer));
}

const Engine::Impl *Engine::pimpl() const {
    return std::launder(reinterpret_cast<const Impl *>(_buffer));
}

void Engine::reset_game() {
    auto &[position, negamax, history] = *pimpl();
    negamax.reset();
    position = Position::initial();
    history.clear();
    _legal_moves = movegen::legals(position);
}

unsigned Engine::search_depth() const {
    return pimpl()->negamax.search_depth();
}

unsigned Engine::thread_count() const {
    return pimpl()->negamax.thread_count();
}

Side Engine::side_to_move() const {
    return pimpl()->position.state().side_to_move();
}

const std::array<Piece, SquareCNT> &Engine::board() const {
    return pimpl()->position.pieces().board();
}

const Array<Move, 256> &Engine::legal_moves() const {
    return _legal_moves;
}

void Engine::stop_move_search() {
    pimpl()->negamax.stop_search();
}

void Engine::wait_while_searching() const {
    pimpl()->negamax.wait_while_searching();
}

bool Engine::is_searching() const {
    return pimpl()->negamax.is_searching();
}

Move Engine::search_result() const {
    return pimpl()->negamax.result();
}

void Engine::set_search_depth(unsigned search_depth) {
    Negamax &negamax = pimpl()->negamax;

#ifdef __cpp_exceptions
    if (negamax.is_searching()) {
        throw std::logic_error("cannot set search depth while searching");
    }
#endif

    negamax.set_search_depth(search_depth);
}

void Engine::set_thread_count(int thread_count) {
    Negamax &negamax = pimpl()->negamax;

#ifdef __cpp_exceptions
    if (negamax.is_searching()) {
        throw std::logic_error("cannot set thread count while searching");
    }
#endif

    negamax.set_thread_count(thread_count);
}

void Engine::start_move_search() {
    auto &[position, negamax, _] = *pimpl();

#ifdef __cpp_exceptions
    if (negamax.is_searching()) {
        throw std::logic_error("already searching");
    }
    if (status() != OnGoing) {
        throw std::logic_error("game over");
    }
#endif

    negamax.start_search(position, _legal_moves);
}

ChessStatus Engine::do_move(const Move move) {
#ifdef __cpp_exceptions
    if (status() != OnGoing) {
        throw std::logic_error("game over");
    }
    if (std::ranges::find(_legal_moves, move) == _legal_moves.end()) {
        throw std::invalid_argument("illegal move");
    }
#endif

    auto &[position, _, history] = *pimpl();
    position.do_legal(move);
    position.trim_history(history);
    _legal_moves = movegen::legals(position);

    return status();
}

void Engine::undo_move() {
    auto &[position, negamax, history] = *pimpl();
    negamax.stop_search();
    position.undo_move_restore_history(history);
    _legal_moves = movegen::legals(position);
    negamax.wait_while_searching();
}

ChessStatus Engine::status() const {
    const Position &position = pimpl()->position;

    if (_legal_moves.size() == 0) {
        if (!position.is_in_check()) {
            return Stalemate;
        }
        if (position.state().side_to_move() == White) {
            return BlackWin;
        } else {
            return WhiteWin;
        }
    }

    if (position.is_50move_draw()) {
        return Draw50Move;
    }
    if (position.is_3fold_repetition()) {
        return Draw3Repetition;
    }
    if (position.is_insufficient_material()) {
        return DrawInsufficientMaterial;
    }

    return OnGoing;
}

} // namespace cheslib
