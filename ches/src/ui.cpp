#include "ui.hpp"
#include "config.hpp"

#include <cstdio>

namespace ches::ui {

namespace cl = ::cheslib;

cl::Square screen_to_square(const ::Vector2 mouse, const cl::Side viewer) {
    assert(::CheckCollisionPointRec(mouse, config::BoardRect));

    const int col = (mouse.x - config::BoardRect.x) / config::SquareSize;
    const int row = (mouse.y - config::BoardRect.y) / config::SquareSize;
    const cl::Square cell = cl::square_of(cl::File(col), cl::Rank(row));

    if (viewer == cl::White) {
        return cl::flip_rank(cell);
    } else {
        return cl::flip_file(cell);
    }
}

::Vector2 square_to_screen(const cl::Square square, const cl::Side viewer) {
    assert(square < cl::SquareCNT);

    const cl::Square cell = (viewer == cl::White) ? cl::flip_rank(square) : cl::flip_file(square);
    const int row = cl::rank_of(cell);
    const int col = cl::file_of(cell);

    return ::Vector2{
        .x = col * config::SquareSize + config::BoardRect.x, //
        .y = row * config::SquareSize + config::BoardRect.y
    };
}

void draw_highlight(const cl::Square square, const cl::Side viewer) {
    assert(square < cl::SquareCNT);
    const auto [x, y] = square_to_screen(square, viewer);
    ::DrawRectangle(x, y, config::SquareSize, config::SquareSize, config::Highlight);
}

void draw_move_hint(const cl::Square from, const cl::Array<cl::Move, 256> &moves, const cl::Side viewer) {
    for (const cl::Move move : moves) {
        if (move.from() == from) {
            const auto [x, y] = square_to_screen(move.to(), viewer);
            ::DrawCircle(
                x + config::SquareSize / 2, y + config::SquareSize / 2, config::SquareSize / 6, config::MoveHint
            );
        }
    }
}

void draw_squares(const cl::Side viewer, const ::Font &font) {
    // draw 64 squares
    ::DrawRectangleRec(config::BoardRect, config::LightSquare);
    for (int row = 0; row < cl::RankCNT; ++row) {
        for (int col = (row % 2 == 0); col < cl::FileCNT; col += 2) {
            const int x = col * config::SquareSize + config::BoardRect.x;
            const int y = row * config::SquareSize + config::BoardRect.y;

            ::DrawRectangle(x, y, config::SquareSize, config::SquareSize, config::DarkSquare);
        }
    }

    // draw rank labels
    for (int row = 0; row < cl::RankCNT; ++row) {
        const int label = '1' + ((viewer == cl::White) ? (cl::Rank8 - row) : row);
        const ::Color color = (row % 2 == 0) ? config::LightSquare : config::DarkSquare;
        const ::Vector2 position{
            .x = config::SquareSize * cl::FileCNT + config::BoardRect.x - ::GetGlyphAtlasRec(font, label).width * 1.5f,
            .y = config::SquareSize * row + config::BoardRect.y
        };

        ::DrawTextCodepoint(font, label, position, config::FontSize, color);
    }

    // draw file labels
    for (int col = 0; col < cl::FileCNT; ++col) {
        const int label = 'a' + ((viewer == cl::White) ? col : (cl::FileH - col));
        const ::Color color = (col % 2 == 0) ? config::LightSquare : config::DarkSquare;
        const ::Vector2 position{
            .x = config::SquareSize * col + config::BoardRect.x + ::GetGlyphAtlasRec(font, label).width / 2,
            .y = config::SquareSize * cl::RankCNT + config::BoardRect.y - config::FontSize
        };

        ::DrawTextCodepoint(font, label, position, config::FontSize, color);
    }
}

void draw_piece(const cl::Piece piece, const ::Vector2 center, const ::Texture2D &pieces_sprite) {
    assert(piece < cl::PieceCNT);

    const ::Vector2 position{
        .x = center.x - config::SquareSize / 2, //
        .y = center.y - config::SquareSize / 2
    };
    const ::Rectangle source{
        .x = (float)piece * config::SquareSize, //
        .y = 0,
        .width = config::SquareSize,
        .height = config::SquareSize
    };

    ::DrawTextureRec(pieces_sprite, source, position, ::WHITE);
}

void draw_pieces(
    const std::array<cl::Piece, cl::SquareCNT> &board, const cl::Side viewer, const ::Texture2D &pieces_sprite
) {
    for (cl::Square sq = cl::SquareA1; sq <= cl::SquareH8; ++sq) {
        const cl::Piece piece = board[sq];
        if (piece >= cl::PieceCNT) {
            continue;
        }

        const ::Rectangle source{
            .x = (float)piece * config::SquareSize, //
            .y = 0,
            .width = config::SquareSize,
            .height = config::SquareSize
        };
        ::DrawTextureRec(pieces_sprite, source, square_to_screen(sq, viewer), ::WHITE);
    }
}

void draw_promotion(const cl::Square at, const ::Texture2D &pieces_sprite) {
    assert(cl::rank_of(at) == cl::Rank1 || cl::rank_of(at) == cl::Rank8);

    const cl::Side side = (cl::rank_of(at) == cl::Rank8) ? cl::White : cl::Black;
    const ::Vector2 square_pos = square_to_screen(at, side);
    ::DrawRectangle(square_pos.x, square_pos.y, config::SquareSize, config::SquareSize * 4, config::DimHighlight);

    for (cl::PieceType type = cl::Knight; type <= cl::Queen; ++type) {
        const ::Rectangle source{
            .x = config::SquareSize * (float)cl::piece_of(side, type),
            .y = 0,
            .width = config::SquareSize,
            .height = config::SquareSize
        };
        const ::Vector2 position{
            .x = square_pos.x, //
            .y = square_pos.y + config::SquareSize * (float)(cl::Queen - type)
        };
        ::DrawTextureRec(pieces_sprite, source, position, ::WHITE);
    }
}

//

namespace {

constexpr char ResultTexts[][18] = {"White won",    "Black won",         "Stalemate draw",
                                    "50-move draw", "3 repetition draw", "Insufficient draw"};

void draw_textbox(const char *const text, const ::Rectangle rect, const ::Font &font) {
    const ::Vector2 size = ::MeasureTextEx(font, text, config::FontSize, 0);
    const ::Vector2 position{
        .x = rect.x + (rect.width - size.x) / 2, //
        .y = rect.y + (rect.height - size.y) / 2
    };
    ::DrawRectangleRec(rect, config::TextBackground);
    ::DrawTextEx(font, text, position, config::FontSize, 0, ::WHITE);
}

} // namespace

void draw_ui_panel(const cl::Engine &engine, const ::Font &font) {
    draw_textbox("New Game", config::NewGameButtonRect, font);
    draw_textbox("Undo Move", config::UndoButtonRect, font);

    char text[20];
    std::sprintf(text, "-    Depth: %2u    +", engine.search_depth());
    draw_textbox(text, config::DepthSpinnerRect, font);

    std::sprintf(text, "-  Threads: %3u  +", engine.thread_count());
    draw_textbox(text, config::ThreadSpinnerRect, font);
}

void draw_search_status(const float time, const ::Font &font) {
    char text[16];
    std::sprintf(text, "Time: %8.3fs", std::min(time, 9999.000f));
    draw_textbox(text, config::StatusRect, font);
}

void draw_gameover_status(const cl::ChessStatus status, const ::Font &font) {
    assert(status != cl::OnGoing);
    draw_textbox(ResultTexts[status - 1], config::StatusRect, font);
}

} // namespace ches::ui
