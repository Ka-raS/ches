#include "cheslib/engine.hpp"
#include "movegen.hpp"
#include "negamax.hpp"

#include <stdexcept>

namespace cheslib {

struct Engine::Impl {
    Position position;
    Negamax negamax;
};

Engine::Engine(const unsigned search_depth, const int thread_count) {
    static_assert(sizeof(_buffer) >= sizeof(Impl));
    static_assert(BufferAlign == alignof(Impl));

    Impl *const impl = new (_buffer) Impl{
        .position{Position::initial()}, //
        .negamax{search_depth, thread_count}
    };

    movegen::legals(impl->position, _legal_moves);
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
    auto &[position, negamax] = *pimpl();
    negamax.reset();
    position = Position::initial();
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

void Engine::start_move_search() {
#ifdef __cpp_exceptions
    if (state() != ChessState::OnGoing) {
        throw std::logic_error("game over");
    }
#endif

    auto &[position, negamax] = *pimpl();
    negamax.start_search(position, _legal_moves);
}

ChessState Engine::do_move(const Move move) {
#ifdef __cpp_exceptions
    if (state() != ChessState::OnGoing) {
        throw std::logic_error("game over");
    }
    if (std::ranges::find(_legal_moves, move) == _legal_moves.end()) {
        throw std::invalid_argument("illegal move");
    }
#endif

    Position &position = pimpl()->position;
    position.do_legal(move);
    position.trim_history();
    movegen::legals(position, _legal_moves);
    return state();
}

ChessState Engine::state() const {
    const Position &position = pimpl()->position;

    if (_legal_moves.size() == 0) {
        if (!position.is_in_check()) {
            return ChessState::Stalemate;
        }
        if (position.state().side_to_move() == White) {
            return ChessState::BlackWin;
        } else {
            return ChessState::WhiteWin;
        }
    }

    if (position.is_50move_draw()) {
        return ChessState::Draw50Move;
    }
    if (position.is_3fold_repetition()) {
        return ChessState::Draw3Repetition;
    }
    if (position.is_insufficient_material()) {
        return ChessState::DrawInsufficientMaterial;
    }

    return ChessState::OnGoing;
}

} // namespace cheslib
