#include "game_states.hpp"
#include "game.hpp"

namespace ches::state {

namespace cl = ::cheslib;

namespace {

constexpr char ResultTexts[][18] = {"White won",         "Black won",    "Stalemate draw",
                                    "Insufficient draw", "50-move draw", "3 repetition draw"};

GameState on_newgame(GameContext &c) {
    c.engine.reset_game();
    c.user = !c.user;
    c.board_ui.set_viewer(c.user);

    if (c.user == cl::White) {
        return SelectingPiece{};
    } else {
        return EnginePlaying{c.engine};
    }
}

} // namespace

StateResult SelectingPiece::update(GameContext &c, const MouseEvent mouse) const {
    if (c.ui_panel.is_newgame_button(mouse.position)) {
        return {
            .next_state = (mouse.left == KeyState::Pressed) ? on_newgame(c) : *this,
            .cursor = ::MOUSE_CURSOR_POINTING_HAND
        };
    }

    const cl::Square square = c.board_ui.screen_to_square(mouse.position);
    if (square >= cl::SquareCNT) {
        return StateResult{*this};
    }

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
    c.board_ui.draw_squares();
    c.board_ui.draw_pieces(c.engine.board());
    c.ui_panel.draw_default();
}

//

DraggingPiece::DraggingPiece(const ::Vector2 mouse_position, const cl::Square selected_piece) :
    _mouse_position{mouse_position},
    _selected_piece{selected_piece} {}

StateResult DraggingPiece::update(GameContext &c, const MouseEvent mouse) const {
    if (mouse.left == KeyState::Holding) {
        return StateResult{DraggingPiece{mouse.position, _selected_piece}, ::MOUSE_CURSOR_RESIZE_ALL};
    }

    assert(mouse.left == KeyState::Released);

    const cl::Square selected = c.board_ui.screen_to_square(mouse.position);
    if (selected >= cl::SquareCNT) {
        return StateResult{SelectingDestination{_selected_piece}};
    }

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
    if (status == cl::ChessStatus::OnGoing) {
        return StateResult{EnginePlaying{c.engine}};
    } else {
        return StateResult{GameOver{status}};
    }

    return StateResult{SelectingDestination{_selected_piece}};
}

void DraggingPiece::draw(const GameContext &c) const {
    c.board_ui.draw_squares();
    c.board_ui.draw_pieces_except(_selected_piece, c.engine.board());
    c.board_ui.draw_move_hint(_selected_piece, c.engine.legal_moves());

    const cl::Piece piece = c.engine.board()[_selected_piece];
    assert(piece < cl::PieceCNT);
    c.board_ui.draw_piece_centered(piece, _mouse_position);
    c.ui_panel.draw_default();
}

//

SelectingDestination::SelectingDestination(const cl::Square selected_piece) :
    _selected_piece{selected_piece} {}

StateResult SelectingDestination::update(GameContext &c, const MouseEvent mouse) const {
    if (c.ui_panel.is_newgame_button(mouse.position)) {
        return StateResult{
            .next_state = (mouse.left == KeyState::Pressed) ? on_newgame(c) : *this,
            .cursor = ::MOUSE_CURSOR_POINTING_HAND
        };
    }

    const cl::Square selected = c.board_ui.screen_to_square(mouse.position);
    if (selected >= cl::SquareCNT) {
        return StateResult{*this};
    }

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
        return StateResult{*this};
    }
    if (mouse.left != KeyState::Pressed) {
        return StateResult{*this, ::MOUSE_CURSOR_POINTING_HAND};
    }
    if (move.is_promotion()) {
        return StateResult{PromotingPawn{_selected_piece, selected}};
    }

    const cl::ChessStatus status = c.engine.do_move(move);
    if (status == cl::ChessStatus::OnGoing) {
        return StateResult{EnginePlaying{c.engine}};
    } else {
        return StateResult{GameOver{status}};
    }
}

void SelectingDestination::draw(const GameContext &c) const {
    c.board_ui.draw_squares();
    c.board_ui.draw_highlight(_selected_piece);
    c.board_ui.draw_pieces(c.engine.board());
    c.board_ui.draw_move_hint(_selected_piece, c.engine.legal_moves());
    c.ui_panel.draw_default();
}

//

PromotingPawn::PromotingPawn(const cl::Square selected_pawn, const cl::Square promotion_square) :
    _selected_pawn{selected_pawn},
    _promotion_square{promotion_square} {}

StateResult PromotingPawn::update(GameContext &c, const MouseEvent mouse) const {
    if (c.ui_panel.is_newgame_button(mouse.position)) {
        return {
            .next_state = (mouse.left == KeyState::Pressed) ? on_newgame(c) : *this,
            .cursor = ::MOUSE_CURSOR_POINTING_HAND
        };
    }

    const cl::PieceType promo = c.ui_panel.promotion_piece_at(mouse.position);
    if (promo >= cl::PieceTypeCNT) {
        return StateResult{*this};
    }
    if (mouse.left != KeyState::Pressed) {
        return StateResult{*this, ::MOUSE_CURSOR_POINTING_HAND};
    }

    const bool is_capture = (c.engine.board()[_promotion_square] < cl::PieceCNT);
    const cl::MoveFlag flag = cl::MoveFlag(cl::KnightPromo | (promo - 1) | (is_capture ? cl::Capture : cl::QuietMove));
    const cl::Move move{_selected_pawn, _promotion_square, flag};
    const cl::ChessStatus status = c.engine.do_move(move);

    if (status == cl::ChessStatus::OnGoing) {
        return StateResult{EnginePlaying{c.engine}};
    } else {
        return StateResult{GameOver{status}};
    }
}

void PromotingPawn::draw(const GameContext &c) const {
    c.board_ui.draw_squares();
    c.board_ui.draw_highlight(_selected_pawn);
    c.board_ui.draw_pieces(c.engine.board());
    c.ui_panel.draw_default();
    c.ui_panel.draw_promotion(c.user);
}

//

EnginePlaying::EnginePlaying(cl::Engine &engine) :
    _start_time(::GetTime()) {
    engine.start_move_search();
    ::DisableEventWaiting();
}

StateResult EnginePlaying::update(GameContext &c, const MouseEvent mouse) const {
    c.ui_panel.set_searched_time((float)::GetTime() - _start_time);

    if (c.ui_panel.is_newgame_button(mouse.position)) {
        if (mouse.left == KeyState::Pressed) {
            ::EnableEventWaiting();
            return StateResult{on_newgame(c), ::MOUSE_CURSOR_POINTING_HAND};
        } else {
            return StateResult{*this, ::MOUSE_CURSOR_POINTING_HAND};
        }
    }

    if (c.engine.is_searching()) {
        return StateResult{*this};
    }

    ::EnableEventWaiting();
    const cl::Move move = c.engine.search_result();
    const cl::ChessStatus status = c.engine.do_move(move);

    if (status == cl::ChessStatus::OnGoing) {
        return StateResult{SelectingPiece{}};
    } else {
        return StateResult{GameOver{status}};
    }
}

void EnginePlaying::draw(const GameContext &c) const {
    c.board_ui.draw_squares();
    c.board_ui.draw_pieces(c.engine.board());
    c.ui_panel.draw_default();
}

//

GameOver::GameOver(const cl::ChessStatus result) :
    _result{result} {
    assert(result != cl::ChessStatus::OnGoing);
}

StateResult GameOver::update(GameContext &c, const MouseEvent mouse) const {
    if (c.ui_panel.is_newgame_button(mouse.position)) {
        return StateResult{
            .next_state = (mouse.left == KeyState::Pressed) ? on_newgame(c) : *this,
            .cursor = ::MOUSE_CURSOR_POINTING_HAND
        };
    }

    return StateResult{*this};
}

void GameOver::draw(const GameContext &c) const {
    c.board_ui.draw_squares();
    c.board_ui.draw_pieces(c.engine.board());
    c.ui_panel.draw_default();
    c.ui_panel.draw_status(ResultTexts[size_t(_result) - 1]);
}

} // namespace ches::state
