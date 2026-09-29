#include "game_states.hpp"
#include "config.hpp"
#include "game.hpp"
#include "ui.hpp"

namespace ches::state {

namespace cl = ::cheslib;

namespace {

StateResult handle_ui_panel(GameContext &c, const MouseEvent mouse, const GameState current_state) {
    if (::CheckCollisionPointRec(mouse.position, config::NewGameButtonRect)) {
        if (mouse.left == KeyState::Pressed) {
            c.engine.reset_game();
            c.user = !c.user;

            if (c.user == cl::White) {
                ::EnableEventWaiting();
                return StateResult{SelectingPiece{}};
            } else {
                return StateResult{EnginePlaying{c.engine}};
            }
        }
        return StateResult{current_state, ::MOUSE_CURSOR_POINTING_HAND};
    }

    if (::CheckCollisionPointRec(mouse.position, config::UndoButtonRect)) {
        if (mouse.left == KeyState::Pressed) {
            ::EnableEventWaiting();
            c.engine.undo_move();
            if (c.user != c.engine.side_to_move()) {
                c.engine.undo_move();

                if (c.user != c.engine.side_to_move()) {
                    // it's the starting position, engine is white to move
                    assert(c.user == cl::Black);
                    return StateResult{EnginePlaying{c.engine}};
                }
            }
            return StateResult{SelectingPiece{}};
        }
        return StateResult{current_state, ::MOUSE_CURSOR_POINTING_HAND};
    }

    if (c.engine.is_searching()) {
        return StateResult{current_state};
    }

    if (::CheckCollisionPointRec(mouse.position, config::DepthSpinnerRect)) {
        const int delta = (mouse.position.x > config::DepthSpinnerRect.width * 0.875f + config::DepthSpinnerRect.x) -
                          (mouse.position.x < config::DepthSpinnerRect.width * 0.125f + config::DepthSpinnerRect.x);
        if (delta != 0) {
            if (mouse.left == KeyState::Pressed) {
                c.engine.set_search_depth(c.engine.search_depth() + delta);
            }
            return StateResult{current_state, ::MOUSE_CURSOR_POINTING_HAND};
        }

    } else if (::CheckCollisionPointRec(mouse.position, config::ThreadSpinnerRect)) {
        const int delta = (mouse.position.x > config::ThreadSpinnerRect.width * 0.875f + config::ThreadSpinnerRect.x) -
                          (mouse.position.x < config::ThreadSpinnerRect.width * 0.125f + config::ThreadSpinnerRect.x);
        if (delta != 0) {
            if (mouse.left == KeyState::Pressed) {
                c.engine.set_thread_count(c.engine.thread_count() + delta);
            }
            return StateResult{current_state, ::MOUSE_CURSOR_POINTING_HAND};
        }
    }

    return StateResult{current_state};
}

} // namespace

SelectingPiece::SelectingPiece(const float previous_search_time) :
    _previous_search_time{previous_search_time} {}

StateResult SelectingPiece::handle(GameContext &c, const MouseEvent mouse) const {
    if (!::CheckCollisionPointRec(mouse.position, config::BoardRect)) {
        return handle_ui_panel(c, mouse, *this);
    }

    const cl::Square square = ui::screen_to_square(mouse.position, c.user);
    const cl::Piece piece = c.engine.board()[square];
    if (piece >= cl::PieceCNT || c.user != cl::side_of(piece)) {
        return StateResult{*this};
    }

    if (mouse.left == KeyState::Pressed) {
        return StateResult{DraggingPiece{mouse.position, square}, ::MOUSE_CURSOR_RESIZE_ALL};
    } else {
        return StateResult{*this, ::MOUSE_CURSOR_POINTING_HAND};
    }
}

void SelectingPiece::draw(const GameContext &c) const {
    ui::draw_squares(c.user, c.assets.font());
    ui::draw_pieces(c.engine.board(), c.user, c.assets.pieces_sprite());
    ui::draw_ui_panel(c.engine, c.assets.font());
    if (_previous_search_time > 0) {
        ui::draw_search_status(_previous_search_time, c.assets.font());
    }
}

//

DraggingPiece::DraggingPiece(const ::Vector2 mouse_position, const cl::Square selected_piece) :
    _mouse_position{mouse_position},
    _selected_piece{selected_piece} {}

StateResult DraggingPiece::handle(GameContext &c, const MouseEvent mouse) const {
    if (mouse.left == KeyState::Holding) {
        return StateResult{DraggingPiece{mouse.position, _selected_piece}, ::MOUSE_CURSOR_RESIZE_ALL};
    }

    assert(mouse.left == KeyState::Released);
    if (!::CheckCollisionPointRec(mouse.position, config::BoardRect)) {
        return StateResult{SelectingDestination{_selected_piece}};
    }

    const cl::Square selected = ui::screen_to_square(mouse.position, c.user);

    cl::Move move = cl::Move::none();
    for (const cl::Move m : c.engine.legal_moves()) {
        if (m.from() == _selected_piece && m.to() == selected) {
            move = m;
            break;
        }
    }

    if (move == cl::Move::none()) {
        return StateResult{SelectingDestination{_selected_piece}};
    }
    if (move.is_promotion()) {
        return StateResult{PromotingPawn{_selected_piece, selected}};
    }

    const cl::ChessStatus status = c.engine.do_move(move);
    if (status == cl::OnGoing) {
        return StateResult{EnginePlaying{c.engine}};
    } else {
        return StateResult{GameOver{status}};
    }
}

void DraggingPiece::draw(const GameContext &c) const {
    ui::draw_squares(c.user, c.assets.font());
    ui::draw_pieces(c.engine.board(), c.user, c.assets.pieces_sprite());
    ui::draw_highlight(_selected_piece, c.user);
    ui::draw_move_hint(_selected_piece, c.engine.legal_moves(), c.user);

    const cl::Piece piece = c.engine.board()[_selected_piece];
    assert(piece < cl::PieceCNT);
    ui::draw_piece(piece, _mouse_position, c.assets.pieces_sprite());
    ui::draw_ui_panel(c.engine, c.assets.font());
}

//

SelectingDestination::SelectingDestination(const cl::Square selected_piece) :
    _selected_piece{selected_piece} {}

StateResult SelectingDestination::handle(GameContext &c, const MouseEvent mouse) const {
    if (!::CheckCollisionPointRec(mouse.position, config::BoardRect)) {
        return handle_ui_panel(c, mouse, *this);
    }

    const cl::Square selected = ui::screen_to_square(mouse.position, c.user);

    {
        const cl::Piece piece = c.engine.board()[selected];
        const bool is_reselecting_piece = (piece < cl::PieceCNT) && (c.user == cl::side_of(piece));
        if (is_reselecting_piece) {
            if (mouse.left == KeyState::Pressed) {
                return StateResult{DraggingPiece{mouse.position, selected}, ::MOUSE_CURSOR_RESIZE_ALL};
            } else {
                return StateResult{*this, ::MOUSE_CURSOR_POINTING_HAND};
            }
        }
    }

    cl::Move move = cl::Move::none();
    for (const cl::Move m : c.engine.legal_moves()) {
        if (m.from() == _selected_piece && m.to() == selected) {
            move = m;
            break;
        }
    }

    if (move == cl::Move::none()) {
        if (mouse.left == KeyState::Pressed) {
            return StateResult{SelectingPiece{}};
        } else {
            return StateResult{*this};
        }
    }
    if (mouse.left != KeyState::Pressed) {
        return StateResult{*this, ::MOUSE_CURSOR_POINTING_HAND};
    }
    if (move.is_promotion()) {
        return StateResult{PromotingPawn{_selected_piece, selected}};
    }

    const cl::ChessStatus status = c.engine.do_move(move);
    if (status == cl::OnGoing) {
        return StateResult{EnginePlaying{c.engine}};
    } else {
        return StateResult{GameOver{status}};
    }
}

void SelectingDestination::draw(const GameContext &c) const {
    ui::draw_squares(c.user, c.assets.font());
    ui::draw_highlight(_selected_piece, c.user);
    ui::draw_pieces(c.engine.board(), c.user, c.assets.pieces_sprite());
    ui::draw_move_hint(_selected_piece, c.engine.legal_moves(), c.user);
    ui::draw_ui_panel(c.engine, c.assets.font());
}

//

PromotingPawn::PromotingPawn(const cl::Square selected_pawn, const cl::Square promotion_square) :
    _selected_pawn{selected_pawn},
    _promotion_square{promotion_square} {}

StateResult PromotingPawn::handle(GameContext &c, const MouseEvent mouse) const {
    if (!::CheckCollisionPointRec(mouse.position, config::BoardRect)) {
        return handle_ui_panel(c, mouse, *this);
    }

    const cl::Square selected = ui::screen_to_square(mouse.position, c.user);
    const cl::MoveFlag promo =
        cl::MoveFlag((c.user == cl::White) ? (4u + cl::rank_of(selected)) : (11u - cl::rank_of(selected)));

    if (cl::KnightPromo > promo || promo > cl::QueenPromo || cl::file_of(selected) != cl::file_of(_promotion_square)) {
        if (mouse.left == KeyState::Pressed) {
            return StateResult{SelectingPiece{}};
        } else {
            return StateResult{*this};
        }
    }
    if (mouse.left != KeyState::Pressed) {
        return StateResult{*this, ::MOUSE_CURSOR_POINTING_HAND};
    }

    const bool is_capture = (c.engine.board()[_promotion_square] < cl::PieceCNT);
    const cl::MoveFlag flag = cl::MoveFlag(promo | (is_capture ? cl::Capture : cl::QuietMove));
    const cl::Move move{_selected_pawn, _promotion_square, flag};
    const cl::ChessStatus status = c.engine.do_move(move);

    if (status == cl::OnGoing) {
        return StateResult{EnginePlaying{c.engine}};
    } else {
        return StateResult{GameOver{status}};
    }
}

void PromotingPawn::draw(const GameContext &c) const {
    ui::draw_squares(c.user, c.assets.font());
    ui::draw_highlight(_selected_pawn, c.user);
    ui::draw_pieces(c.engine.board(), c.user, c.assets.pieces_sprite());
    ui::draw_ui_panel(c.engine, c.assets.font());
    ui::draw_promotion(_promotion_square, c.assets.pieces_sprite());
}

//

EnginePlaying::EnginePlaying(cl::Engine &engine) :
    _start_time(::GetTime()) {
    engine.start_move_search();
    ::DisableEventWaiting();
}

StateResult EnginePlaying::handle(GameContext &c, const MouseEvent mouse) const {
    if (c.engine.is_searching()) {
        return handle_ui_panel(c, mouse, *this);
    }

    ::EnableEventWaiting();
    const cl::Move move = c.engine.search_result();
    const cl::ChessStatus status = c.engine.do_move(move);

    if (status == cl::OnGoing) {
        return StateResult{SelectingPiece{(float)::GetTime() - _start_time}};
    } else {
        return StateResult{GameOver{status}};
    }
}

void EnginePlaying::draw(const GameContext &c) const {
    ui::draw_squares(c.user, c.assets.font());
    ui::draw_pieces(c.engine.board(), c.user, c.assets.pieces_sprite());
    ui::draw_ui_panel(c.engine, c.assets.font());
    ui::draw_search_status((float)::GetTime() - _start_time, c.assets.font());
}

//

GameOver::GameOver(const cl::ChessStatus result) :
    _result{result} {
    assert(result != cl::OnGoing);
}

StateResult GameOver::handle(GameContext &c, const MouseEvent mouse) const {
    return handle_ui_panel(c, mouse, *this);
}

void GameOver::draw(const GameContext &c) const {
    ui::draw_squares(c.user, c.assets.font());
    ui::draw_pieces(c.engine.board(), c.user, c.assets.pieces_sprite());
    ui::draw_ui_panel(c.engine, c.assets.font());
    ui::draw_gameover_status(_result, c.assets.font());
}

} // namespace ches::state
