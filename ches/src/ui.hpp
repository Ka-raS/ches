#pragma once

#include <array>

#include <cheslib/engine.hpp>
#include <raylib.h>

namespace ches::ui {

cheslib::Square screen_to_square(::Vector2 mouse, cheslib::Side viewer);

::Vector2 square_to_screen(cheslib::Square square, cheslib::Side viewer);

void draw_squares(cheslib::Side viewer, const ::Font &font);

void draw_highlight(cheslib::Square square, cheslib::Side viewer);

void draw_move_hint(cheslib::Square from, const cheslib::Array<cheslib::Move, 256> &moves, cheslib::Side viewer);

void draw_piece(cheslib::Piece piece, ::Vector2 center, const ::Texture2D &pieces_sprite);

void draw_pieces(
    const std::array<cheslib::Piece, cheslib::SquareCNT> &board, cheslib::Side viewer, const ::Texture2D &pieces_sprite
);

void draw_promotion(const cheslib::Square at, const ::Texture2D &pieces_sprite);

//

void draw_ui_panel(const cheslib::Engine &engine, const ::Font &font);

void draw_search_status(const float time, const ::Font &font);

void draw_gameover_status(const cheslib::ChessStatus status, const ::Font &font);

} // namespace ches::ui
