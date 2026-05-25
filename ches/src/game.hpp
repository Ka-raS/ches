#pragma once

#include <thread>

#include <raylib.h>

#include "cheslib/engine.hpp"

namespace ches {

namespace cl = cheslib;

enum class State {
    SelectingPiece,
    SelectingDestination,
    PromotingPawn,
    EnginePlaying,
    GameOver
};

constexpr ::Rectangle BoardRectangle{
    .x = 88, //
    .y = 88,
    .width = 1024,
    .height = 1024
};
constexpr int SquareSize = BoardRectangle.width / 8;

constexpr ::Rectangle UIPanelRectangle{
    .x = 1200, //
    .y = 88,
    .width = 312,
    .height = 1024
};

// inside UIPanelRectangle:
constexpr ::Rectangle PromoSelectionRectangle{
    .x = UIPanelRectangle.x + (UIPanelRectangle.width - SquareSize) / 2, //
    .y = 1200 - 88 - SquareSize * 4 - (UIPanelRectangle.width - SquareSize) / 2,
    .width = SquareSize,
    .height = SquareSize * 4
};

constexpr ::Rectangle NewGameButtonRectangle{
    .x = UIPanelRectangle.x + 50, //
    .y = UIPanelRectangle.y + 50,
    .width = UIPanelRectangle.width - 100,
    .height = 40
};

constexpr int FontSize = 32;

class Game {
  public:
    Game() = default;

    void run() {
        ::SetConfigFlags(::FLAG_WINDOW_RESIZABLE | ::FLAG_MSAA_4X_HINT);
        ::InitWindow(1600, 1200, "Ches");
        ::EnableEventWaiting();
        rescale();

        _board_texture = ::LoadTexture("assets/pieces-spritesheet.png");
        ::SetTextureFilter(_board_texture, TEXTURE_FILTER_BILINEAR);

        _font = ::LoadFontEx("assets/NotoSans-Bold.ttf", FontSize, nullptr, 0);
        ::SetTextureFilter(_font.texture, TEXTURE_FILTER_BILINEAR);

        while (!::WindowShouldClose()) {
            update();
            render();
        }

        ::UnloadTexture(_board_texture);
        ::UnloadFont(_font);
        ::CloseWindow();
    }

    void rescale() {
        float width = (float)::GetScreenWidth();
        float height = (float)::GetScreenHeight();
        float scale = std::min(width / 1600, height / 1200);

        _camera.zoom = scale;
        _camera.offset.x = (width - 1600 * scale) / 2;
        _camera.offset.y = (height - 1200 * scale) / 2;
    }

    void set_cursor(::MouseCursor cursor) {
        if (_cursor != cursor) {
            _cursor = cursor;
            ::SetMouseCursor(cursor);
        }
    }

    cl::Square screen_to_square(::Vector2 mouse, cl::Side side_to_move) const {
        int file = (mouse.x - BoardRectangle.x) / 128;
        int rank = (mouse.y - BoardRectangle.y) / 128;

        if (side_to_move == cl::White) {
            rank = cl::Rank8 - rank;
        } else {
            file = cl::FileH - file;
        }

        return cl::types::square_of(cl::File(file), cl::Rank(rank));
    }

    cl::Move find_move(cl::Square from, cl::Square to) const {
        for (cl::Move move : _engine.legal_moves()) {
            if (move.from() == from && move.to() == to) {
                return move;
            }
        }
        return cl::Move::none();
    }

    void update() {
        if (::IsWindowResized()) {
            rescale();
        }
        bool testing = false;

        switch (_state) {
        case State::SelectingPiece: {
            const ::Vector2 mouse = ::GetScreenToWorld2D(::GetMousePosition(), _camera);
            const bool clicked = ::IsMouseButtonPressed(::MOUSE_BUTTON_LEFT);

            if (::CheckCollisionPointRec(mouse, NewGameButtonRectangle)) {
                set_cursor(::MOUSE_CURSOR_POINTING_HAND);

                if (clicked) {
                    _engine.reset_game();
                    _user = !_user;
                    _selected_piece = cl::SquareCNT;
                    _state = State::SelectingPiece;
                }

                break;
            }

            if (!::CheckCollisionPointRec(mouse, BoardRectangle)) {
                set_cursor(::MOUSE_CURSOR_DEFAULT);
                break;
            }

            cl::Square square = screen_to_square(mouse, _user);
            cl::Piece piece = _engine.board()[square];

            if (piece >= cl::PieceCNT || _user != cl::types::side_of(piece)) {
                set_cursor(::MOUSE_CURSOR_DEFAULT);
                break;
            }

            set_cursor(::MOUSE_CURSOR_POINTING_HAND);

            if (clicked) {
                _state = State::SelectingDestination;
                _selected_piece = square;
            }

            break;
        }

        case State::SelectingDestination: {
            const ::Vector2 mouse = ::GetScreenToWorld2D(::GetMousePosition(), _camera);
            const bool clicked = ::IsMouseButtonPressed(::MOUSE_BUTTON_LEFT);

            if (::CheckCollisionPointRec(mouse, NewGameButtonRectangle)) {
                set_cursor(::MOUSE_CURSOR_POINTING_HAND);

                if (clicked) {
                    _engine.reset_game();
                    _user = !_user;
                    _selected_piece = cl::SquareCNT;
                    _state = State::SelectingPiece;
                }

                break;
            }

            if (!::CheckCollisionPointRec(mouse, BoardRectangle)) {
                set_cursor(::MOUSE_CURSOR_DEFAULT);
                break;
            }

            const cl::Square dest_square = screen_to_square(mouse, _user);
            const cl::Piece piece = _engine.board()[dest_square];
            const bool is_our_piece = piece < cl::PieceCNT && _user == cl::types::side_of(piece);
            const cl::Move move = find_move(_selected_piece, dest_square);

            // if mouse not at a clickable
            if (move == cl::Move::none() && !is_our_piece) {
                set_cursor(::MOUSE_CURSOR_DEFAULT);

                if (clicked) {
                    _selected_piece = cl::SquareCNT;
                    _state = State::SelectingPiece;
                }

                break;
            }

            set_cursor(::MOUSE_CURSOR_POINTING_HAND);

            // if hovering a clickable
            if (!clicked) {
                break;
            }

            assert((move == cl::Move::none()) == is_our_piece);

            // if clicked one of our pieces
            if (is_our_piece) {
                _selected_piece = dest_square;
                break;
            }

            if (move.is_promotion()) {
                _state = State::PromotingPawn;
                _selected_destination = dest_square;
                break;
            }

            _engine.do_move(move);

            if (_engine.is_game_over()) {
                _state = State::GameOver;
            } else if (testing) {
                _user = !_user;
                _selected_piece = cl::SquareCNT;
                _state = State::SelectingPiece;
            } else {
                _selected_piece = cl::SquareCNT;
                _state = State::EnginePlaying;
                _engine.start_move_search();
            }

            break;
        }

        case State::PromotingPawn: {
            const ::Vector2 mouse = ::GetScreenToWorld2D(::GetMousePosition(), _camera);

            const bool is_hovering_promo = ::CheckCollisionPointRec(mouse, PromoSelectionRectangle);
            const bool is_hovering_new_game = ::CheckCollisionPointRec(mouse, NewGameButtonRectangle);

            if (is_hovering_promo || is_hovering_new_game) {
                set_cursor(::MOUSE_CURSOR_POINTING_HAND);
            } else {
                set_cursor(::MOUSE_CURSOR_DEFAULT);
            }

            if (!::IsMouseButtonPressed(::MOUSE_BUTTON_LEFT)) {
                break;
            }

            if (is_hovering_new_game) {
                _engine.reset_game();
                _user = !_user;
                _selected_piece = cl::SquareCNT;
                _state = State::SelectingPiece;
                break;
            }

            if (!is_hovering_promo) {
                _selected_piece = _selected_destination = cl::SquareCNT;
                _state = State::SelectingPiece;
                break;
            }

            int index = (mouse.y - PromoSelectionRectangle.y) / SquareSize;
            bool is_capture = _engine.board()[_selected_destination] < cl::PieceCNT;
            cl::MoveFlag flag = cl::MoveFlag(cl::KnightPromo + index + is_capture * cl::Capture);
            cl::Move move(_selected_piece, _selected_destination, flag);

            _engine.do_move(move);

            if (_engine.is_game_over()) {
                _state = State::GameOver;
            } else if (testing) {
                _user = !_user;
                _selected_piece = cl::SquareCNT;
                _state = State::SelectingPiece;
            } else {
                _state = State::EnginePlaying;
                _engine.start_move_search();
            }

            break;
        }

        case State::EnginePlaying: {
            set_cursor(::MOUSE_CURSOR_DEFAULT);
            if (_engine.is_searching()) {
                break;
            }

            _engine.do_move(_engine.search_result());
            _state = _engine.is_game_over() ? State::GameOver : State::SelectingPiece;
            _selected_piece = cl::SquareCNT;

            break;
        }

        case State::GameOver: {
            const ::Vector2 mouse = ::GetScreenToWorld2D(::GetMousePosition(), _camera);
            const bool clicked = ::IsMouseButtonPressed(::MOUSE_BUTTON_LEFT);

            if (::CheckCollisionPointRec(mouse, NewGameButtonRectangle)) {
                set_cursor(::MOUSE_CURSOR_POINTING_HAND);

                if (clicked) {
                    _engine.reset_game();
                    _selected_piece = cl::SquareCNT;
                    _user = !_user;
                    if (_user == cl::Black) {
                        _state = State::EnginePlaying;
                        _engine.start_move_search();
                    } else {
                        _state = State::SelectingPiece;
                    }
                }

                break;
            }

            set_cursor(::MOUSE_CURSOR_DEFAULT);
            break;
        }
        } // switch
    }

    void render() const {
        ::BeginDrawing();
        ::ClearBackground(::Color{0x30, 0x2E, 0x2B, 0xFF}); // #302E2BFF
        ::BeginMode2D(_camera);

        draw_board();
        draw_ui();

        ::EndMode2D();
        ::EndDrawing();
    }

    ::Vector2 square_to_screen(cl::Square square) const {
        const cl::Rank rank = cl::types::rank_of(square);
        const cl::File file = cl::types::file_of(square);

        int col = (_user == cl::White) ? file : cl::FileH - file;
        int row = (_user == cl::White) ? cl::Rank8 - rank : rank;

        return {.x = col * SquareSize + BoardRectangle.x, .y = row * SquareSize + BoardRectangle.y};
    }

    void draw_board() const {
        constexpr ::Color Light{0xEF, 0xD8, 0xB4, 0xFF}; // #EFD8B4FF
        constexpr ::Color Dark{0xB4, 0x87, 0x62, 0xFF};  // #B48762FF

        // draw squares
        for (int row = 0; row < cl::RankCNT; ++row) {
            for (int col = 0; col < cl::FileCNT; ++col) {
                int x = col * SquareSize + BoardRectangle.x;
                int y = row * SquareSize + BoardRectangle.y;
                ::Color color = ((col % 2) != (row % 2)) ? Dark : Light;
                ::DrawRectangle(x, y, SquareSize, SquareSize, color);
            }
        }

        // draw rank labels
        for (cl::Rank rank = cl::Rank1; rank <= cl::Rank8; ++rank) {
            constexpr int col = cl::FileCNT;
            int row = (_user == cl::White) ? cl::Rank8 - rank : rank;

            char label[2] = {char('1' + rank), '\0'};
            ::Vector2 label_size = ::MeasureTextEx(_font, label, _font.baseSize, 0);

            float x = col * SquareSize + BoardRectangle.x - label_size.x * 1.5f;
            float y = row * SquareSize + BoardRectangle.y;
            ::Color color = (row % 2 == 0) ? Light : Dark;

            ::DrawTextEx(_font, label, {x, y}, _font.baseSize, 0, color);
        }

        // draw file labels
        for (cl::File file = cl::FileA; file <= cl::FileH; ++file) {
            constexpr int row = cl::RankCNT;
            int col = (_user == cl::White) ? file : cl::FileH - file;

            char text[2] = {char('a' + file), '\0'};
            ::Vector2 text_size = ::MeasureTextEx(_font, text, _font.baseSize, 0);

            float x = col * SquareSize + BoardRectangle.x + text_size.x / 2;
            float y = row * SquareSize + BoardRectangle.y - text_size.y;
            ::Color color = (col % 2 == 0) ? Light : Dark;

            ::DrawTextEx(_font, text, {x, y}, _font.baseSize, 0, color);
        }

        // draw legal moves if a piece is selected
        if (_selected_piece != cl::SquareCNT) {
            ::Vector2 position = square_to_screen(_selected_piece);
            ::DrawRectangleV(position, {SquareSize, SquareSize}, ::Color{0xAB, 0xCC, 0x20, 0x80}); // #ABCC2080

            for (cl::Move move : _engine.legal_moves()) {
                if (move.from() != _selected_piece) {
                    continue;
                }
                ::Vector2 position = square_to_screen(move.to());
                ::DrawCircle(
                    position.x + SquareSize / 2, position.y + SquareSize / 2, SquareSize / 6, ::Color{18, 82, 28, 121}
                ); // #20CCAB40
            }
        }

        // draw pieces
        for (cl::Square sq = cl::SquareA1; sq <= cl::SquareH8; ++sq) {
            const cl::Piece piece = _engine.board()[sq];
            if (piece >= cl::PieceCNT) {
                continue;
            }

            ::Vector2 position = square_to_screen(sq);
            ::Rectangle source{.x = (float)piece * SquareSize, .y = 0, .width = SquareSize, .height = SquareSize};
            ::DrawTextureRec(_board_texture, source, position, ::WHITE);
        }
    }

    void draw_ui() const {
        ::DrawRectangleRec(UIPanelRectangle, ::Color{0x20, 0x20, 0x20, 0x80}); // #20202080

        { // draw new game button
            ::Vector2 text_size = ::MeasureTextEx(_font, "New Game", _font.baseSize, 0);
            ::Vector2 text_pos{
                .x = NewGameButtonRectangle.x + (NewGameButtonRectangle.width - text_size.x) / 2,
                .y = NewGameButtonRectangle.y + (NewGameButtonRectangle.height - text_size.y) / 2
            };
            ::DrawRectangleRec(NewGameButtonRectangle, ::Color{0x20, 0x20, 0x20, 0x80}); // #20202080
            ::DrawTextEx(_font, "New Game", text_pos, _font.baseSize, 0, ::WHITE);
        }

        if (_state == State::PromotingPawn) {
            ::DrawRectangleRec(PromoSelectionRectangle, ::Color{0x20, 0x20, 0x20, 0x80}); // #20202080
            for (cl::PieceType type = cl::Knight; type <= cl::Queen; ++type) {
                cl::Piece piece = cl::types::piece_of(_user, type);
                ::Rectangle source{.x = (float)piece * SquareSize, .y = 0, .width = SquareSize, .height = SquareSize};
                ::Vector2 position{
                    .x = PromoSelectionRectangle.x, .y = PromoSelectionRectangle.y + (type - cl::Knight) * SquareSize
                };
                ::DrawTextureRec(_board_texture, source, position, ::WHITE);
            }
        }
    }

    State _state{State::SelectingPiece};
    cl::Square _selected_piece{cl::SquareCNT};
    cl::Square _selected_destination{cl::SquareCNT};

    cl::Engine _engine{5, -2};
    cl::Side _user{cl::White};

    ::Camera2D _camera{.zoom = 1.0F};
    ::Texture2D _board_texture{};
    ::Font _font{};
    ::MouseCursor _cursor{MOUSE_CURSOR_DEFAULT};
};

} // namespace ches
