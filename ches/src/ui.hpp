#pragma once

#include <array>
#include <memory>

#include <cheslib/array.hpp>
#include <cheslib/move.hpp>
#include <raylib.h>

namespace ches {

// TODO: struct params

class BoardUI {
  public:
    BoardUI(
        int square_size, ::Vector2 position, ::Color dark_square, ::Color light_square, ::Color highlight,
        ::Color move_hint, cheslib::Side viewer, std::shared_ptr<::Font> font,
        std::shared_ptr<::Texture2D> pieces_sprite
    );

    void switch_viewer();
    void set_viewer(cheslib::Side viewer);

    /// @return `cheslib::SquareCNT` if `mouse` not on board
    cheslib::Square screen_to_square(::Vector2 mouse) const;
    ::Vector2 square_to_screen(cheslib::Square square) const;

    void draw_squares() const;
    void draw_highlight(cheslib::Square square) const;
    void draw_move_hint(cheslib::Square from, const cheslib::Array<cheslib::Move, 256> &moves) const;

    void draw_pieces(const std::array<cheslib::Piece, cheslib::SquareCNT> &board) const;
    void draw_pieces_except(cheslib::Square dismiss, const std::array<cheslib::Piece, cheslib::SquareCNT> &board) const;
    void draw_piece(cheslib::Piece piece, ::Vector2 position) const;
    void draw_piece_centered(cheslib::Piece piece, ::Vector2 center) const;

  private:
    std::shared_ptr<::Font> _font;
    std::shared_ptr<::Texture2D> _pieces_sprite;

    int _square_size;
    ::Vector2 _position;
    ::Color _dark_square;
    ::Color _light_square;
    ::Color _highlight;
    ::Color _move_hint;
    cheslib::Side _viewer;
};

class UIPanel {
  public:
    UIPanel(
        ::Rectangle rect, ::Color background, std::shared_ptr<::Font> font, std::shared_ptr<::Texture2D> pieces_sprite
    );

    void set_searched_time(float seconds);

    /// @return `cheslib::PieceTypeCNT` if `mouse` not on a promotion piece
    cheslib::PieceType promotion_piece_at(::Vector2 mouse) const;
    bool is_newgame_button(::Vector2 mouse) const;

    void draw_default() const;
    void draw_promotion(cheslib::Side side) const;
    void draw_status(const char *text) const;

  private:
    void draw_text(const char *text, const ::Rectangle &rect) const;

  private:
    std::shared_ptr<::Font> _font;
    std::shared_ptr<::Texture2D> _pieces_sprite;

    ::Rectangle _rect;
    ::Rectangle _new_game;
    ::Rectangle _search_info;
    ::Rectangle _status;
    ::Rectangle _promo_select;
    ::Color _background;
    float _searched_time;
};

} // namespace ches
