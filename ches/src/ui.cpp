#include "ui.hpp"

#include <format>

namespace ches {

namespace cl = ::cheslib;

BoardUI::BoardUI(
    const int square_size, const ::Vector2 position, const ::Color dark_square, const ::Color light_square,
    const ::Color highlight, const ::Color move_hint, const cl::Side viewer, std::shared_ptr<::Font> font,
    std::shared_ptr<::Texture2D> pieces_sprite
) :
    _font{std::move(font)},
    _pieces_sprite{std::move(pieces_sprite)},
    _square_size{square_size},
    _position{position},
    _dark_square{dark_square},
    _light_square{light_square},
    _highlight{highlight},
    _move_hint{move_hint},
    _viewer{viewer} {}

void BoardUI::set_viewer(const cl::Side viewer) {
    _viewer = viewer;
}

void BoardUI::switch_viewer() {
    _viewer = !_viewer;
}

cl::Square BoardUI::screen_to_square(const ::Vector2 mouse) const {
    const int col = (mouse.x - _position.x) / _square_size;
    const int row = (mouse.y - _position.y) / _square_size;
    if (0 > col || col >= cl::FileCNT || 0 > row || row >= cl::RankCNT) {
        return cl::SquareCNT;
    }

    const cl::Square cell = cl::square_of(cl::File(col), cl::Rank(row));
    if (_viewer == cl::White) {
        return cl::flip_rank(cell);
    } else {
        return cl::flip_file(cell);
    }
}

::Vector2 BoardUI::square_to_screen(const cl::Square square) const {
    assert(square < cl::SquareCNT);

    const cl::Square cell = (_viewer == cl::White) ? cl::flip_rank(square) : cl::flip_file(square);
    const int row = cl::rank_of(cell);
    const int col = cl::file_of(cell);

    return ::Vector2{
        .x = col * _square_size + _position.x, //
        .y = row * _square_size + _position.y
    };
}

void BoardUI::draw_highlight(const cl::Square square) const {
    assert(square < cl::SquareCNT);

    const auto [x, y] = square_to_screen(square);
    ::DrawRectangle(x, y, _square_size, _square_size, _highlight);
}

void BoardUI::draw_move_hint(const cl::Square from, const cl::Array<cl::Move, 256> &moves) const {
    for (const cl::Move move : moves) {
        if (move.from() == from) {
            const auto [x, y] = square_to_screen(move.to());
            ::DrawCircle(x + _square_size / 2, y + _square_size / 2, _square_size / 6, _move_hint);
        }
    }
}

void BoardUI::draw_squares() const {
    // draw 64 squares
    ::DrawRectangle(_position.x, _position.y, _square_size * cl::FileCNT, _square_size * cl::RankCNT, _light_square);
    for (int row = 0; row < cl::RankCNT; ++row) {
        for (int col = (row % 2 == 0); col < cl::FileCNT; col += 2) {
            const int x = col * _square_size + _position.x;
            const int y = row * _square_size + _position.y;

            ::DrawRectangle(x, y, _square_size, _square_size, _dark_square);
        }
    }

    // draw rank labels
    for (int row = 0; row < cl::RankCNT; ++row) {
        const int rank = (_viewer == cl::White) ? (cl::Rank8 - row) : row;
        const char label[2] = {char('1' + rank), '\0'};
        const ::Vector2 label_size = ::MeasureTextEx(*_font, label, _font->baseSize, 0);

        constexpr int col = cl::FileCNT;
        const float x = col * _square_size + _position.x - label_size.x * 1.5f;
        const float y = row * _square_size + _position.y;
        const ::Color color = (row % 2 == 0) ? _light_square : _dark_square;

        ::DrawTextEx(*_font, label, ::Vector2{x, y}, _font->baseSize, 0, color);
    }

    // draw file labels
    for (int col = 0; col < cl::FileCNT; ++col) {
        const int file = (_viewer == cl::White) ? col : (cl::FileH - col);
        const char label[2] = {char('a' + file), '\0'};
        const ::Vector2 label_size = ::MeasureTextEx(*_font, label, _font->baseSize, 0);

        constexpr int row = cl::RankCNT;
        const float x = col * _square_size + _position.x + label_size.x / 2;
        const float y = row * _square_size + _position.y - label_size.y;
        const ::Color color = (col % 2 == 0) ? _light_square : _dark_square;

        ::DrawTextEx(*_font, label, ::Vector2{x, y}, _font->baseSize, 0, color);
    }
}

void BoardUI::draw_pieces(const std::array<cl::Piece, cl::SquareCNT> &board) const {
    for (cl::Square sq = cl::SquareA1; sq <= cl::SquareH8; ++sq) {
        const cl::Piece piece = board[sq];
        if (piece < cl::PieceCNT) {
            draw_piece(piece, square_to_screen(sq));
        }
    }
}

void BoardUI::draw_pieces_except(const cl::Square dismiss, const std::array<cl::Piece, cl::SquareCNT> &board) const {
    for (cl::Square sq = cl::SquareA1; sq <= cl::SquareH8; ++sq) {
        const cl::Piece piece = board[sq];
        if (piece < cl::PieceCNT && sq != dismiss) {
            draw_piece(piece, square_to_screen(sq));
        }
    }
}

void BoardUI::draw_piece_centered(const cl::Piece piece, const ::Vector2 center) const {
    const ::Vector2 position{
        .x = center.x - _pieces_sprite->height / 2, //
        .y = center.y - _pieces_sprite->height / 2
    };
    draw_piece(piece, position);
}

void BoardUI::draw_piece(const cl::Piece piece, const ::Vector2 position) const {
    assert(piece < cl::PieceCNT);

    const ::Rectangle source{
        .x = (float)piece * _pieces_sprite->height, //
        .y = 0,
        .width = (float)_pieces_sprite->height,
        .height = (float)_pieces_sprite->height
    };

    ::DrawTextureRec(*_pieces_sprite, source, position, ::WHITE);
}

//

UIPanel::UIPanel(
    const ::Rectangle rect, const ::Color background, std::shared_ptr<::Font> font,
    std::shared_ptr<::Texture2D> pieces_sprite
) :
    _font{std::move(font)},
    _pieces_sprite{std::move(pieces_sprite)},
    _rect{rect},
    _new_game{
        .x = rect.x + 50, //
        .y = rect.y + 50,
        .width = rect.width - 100,
        .height = 40
    },
    _search_info{
        .x = rect.x + 50, //
        .y = rect.y + 100,
        .width = rect.width - 100,
        .height = 40
    },
    _status{
        .x = rect.x + 50, //
        .y = rect.y + 150,
        .width = rect.width - 100,
        .height = 40
    },
    _promo_select{
        .x = rect.x + (rect.width - _pieces_sprite->height) / 2, //
        .y = rect.y + rect.height - _pieces_sprite->height * 4 - (rect.width - _pieces_sprite->height) / 2,
        .width = (float)_pieces_sprite->height,
        .height = (float)_pieces_sprite->height * 4
    },
    _background{background},
    _searched_time{0.0f} {};

void UIPanel::set_searched_time(float seconds) {
    _searched_time = seconds;
}

bool UIPanel::is_newgame_button(const ::Vector2 mouse) const {
    return ::CheckCollisionPointRec(mouse, _new_game);
}

cl::PieceType UIPanel::promotion_piece_at(const ::Vector2 mouse) const {
    if (!::CheckCollisionPointRec(mouse, _promo_select)) {
        return cl::PieceTypeCNT;
    }

    const cl::PieceType type = cl::PieceType(1 + (mouse.y - _promo_select.y) / _promo_select.width);
    assert(cl::Knight <= type && type <= cl::Queen);
    return type;
}

void UIPanel::draw_default() const {
    ::DrawRectangleRec(_rect, _background);
    draw_text("New Game", _new_game);
    draw_text(std::format("Searched: {:.2f}s", _searched_time).c_str(), _search_info);
}

void UIPanel::draw_status(const char *const text) const {
    draw_text(text, _status);
}

void UIPanel::draw_promotion(const cl::Side side) const {
    for (cl::PieceType type = cl::Knight; type <= cl::Queen; ++type) {
        const cl::Piece piece = cl::piece_of(side, type);
        const ::Rectangle source{
            .x = (float)piece * _pieces_sprite->height,
            .y = 0,
            .width = (float)_pieces_sprite->height,
            .height = (float)_pieces_sprite->height
        };
        const ::Vector2 position{
            .x = _promo_select.x, //
            .y = _promo_select.y + (type - cl::Knight) * _promo_select.width
        };

        ::DrawTextureRec(*_pieces_sprite, source, position, ::WHITE);
    }
}

void UIPanel::draw_text(const char *const text, const ::Rectangle &rect) const {
    const ::Vector2 size = ::MeasureTextEx(*_font, text, _font->baseSize, 0);
    const ::Vector2 position{
        .x = rect.x + (rect.width - size.x) / 2, //
        .y = rect.y + (rect.height - size.y) / 2
    };
    ::DrawRectangleRec(rect, _background);
    ::DrawTextEx(*_font, text, position, _font->baseSize, 0, ::WHITE);
}

} // namespace ches
