#pragma once

#include <raylib.h>

namespace ches::config {

constexpr inline unsigned EngineSearchDepth = 10;
constexpr inline unsigned EngineThreadCount = 0; // hardware concurrency / 2
constexpr inline unsigned EngineTranspositionsKiB = 256 * 1024;

constexpr inline int WindowWidth = 1024;
constexpr inline int WindowHeight = 1088;
constexpr inline char GameTitle[] = "Ches";
constexpr inline int WindowFlags = ::FLAG_WINDOW_RESIZABLE | ::FLAG_MSAA_4X_HINT | ::FLAG_VSYNC_HINT;

constexpr inline ::Color Background{0x30, 0x2E, 0x2B, 0xFF};     // #302E2BFF
constexpr inline ::Color LightSquare{0xEF, 0xD8, 0xB4, 0xFF};    // #EFD8B4FF
constexpr inline ::Color DarkSquare{0xB4, 0x87, 0x62, 0xFF};     // #B48762FF
constexpr inline ::Color Highlight{0xAB, 0xCC, 0x20, 0x80};      // #ABCC2080
constexpr inline ::Color DimHighlight{0x26, 0x24, 0x21, 0xB2};   // #262421B2
constexpr inline ::Color MoveHint{0x12, 0x52, 0x1C, 0x79};       // #12521C79
constexpr inline ::Color TextBackground{0x27, 0x27, 0x27, 0xFF}; // #272727FF

constexpr inline char IconPath[] = "assets/icon.png";
constexpr inline char FontPath[] = "assets/NotoSans-Bold.ttf";
constexpr inline char PiecesSpritePath[] = "assets/pieces-spritesheet.png";

constexpr inline int FontSize = 32;
constexpr inline int SquareSize = 128;

constexpr inline ::Rectangle UIPanelRect{
    .x = 0, //
    .y = 0,
    .width = WindowWidth,
    .height = 64
};

constexpr inline ::Rectangle BoardRect = {
    .x = 0, //
    .y = UIPanelRect.height,
    .width = SquareSize * 8,
    .height = SquareSize * 8
};

constexpr inline ::Rectangle NewGameButtonRect{
    .x = 8, //
    .y = 8,
    .width = UIPanelRect.width / 5 - 16,
    .height = UIPanelRect.height - 16
};

constexpr inline ::Rectangle UndoButtonRect{
    .x = UIPanelRect.width * 0.2f + 8, //
    .y = 8,
    .width = NewGameButtonRect.width,
    .height = NewGameButtonRect.height
};

constexpr inline ::Rectangle StatusRect{
    .x = UIPanelRect.width * 0.4f + 8, //
    .y = 8,
    .width = NewGameButtonRect.width,
    .height = NewGameButtonRect.height
};

constexpr inline ::Rectangle DepthStepperRect{
    .x = UIPanelRect.width * 0.6f + 8, //
    .y = 8,
    .width = NewGameButtonRect.width,
    .height = NewGameButtonRect.height
};

constexpr inline ::Rectangle ThreadStepperRect{
    .x = UIPanelRect.width * 0.8f + 8, //
    .y = 8,
    .width = NewGameButtonRect.width,
    .height = NewGameButtonRect.height
};

} // namespace ches::config
