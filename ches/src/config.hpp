#pragma once

#include <raylib.h>

namespace ches::config {

constexpr inline unsigned EngineDepth = 6;
constexpr inline int EngineThreadCount = -2; // hardware concurrency - 2

constexpr inline int FPSTarget = 120;
constexpr inline int WindowWidth = 1600;
constexpr inline int WindowHeight = 1200;
constexpr inline char GameTitle[] = "Ches";
constexpr inline ::ConfigFlags WindowConfigs = ::ConfigFlags(::FLAG_WINDOW_RESIZABLE | ::FLAG_MSAA_4X_HINT);

constexpr inline ::Color Background{0x30, 0x2E, 0x2B, 0xFF};      // #302E2BFF
constexpr inline ::Color LightSquare{0xEF, 0xD8, 0xB4, 0xFF};     // #EFD8B4FF
constexpr inline ::Color DarkSquare{0xB4, 0x87, 0x62, 0xFF};      // #B48762FF
constexpr inline ::Color Highlight{0xAB, 0xCC, 0x20, 0x80};       // #ABCC2080
constexpr inline ::Color MoveHint{0x12, 0x52, 0x1C, 0x79};        // #12521C79
constexpr inline ::Color PanelBackground{0x20, 0x20, 0x20, 0x80}; // #20202080

constexpr inline char IconPath[] = "assets/icon.png";
constexpr inline char FontPath[] = "assets/NotoSans-Bold.ttf";
constexpr inline char PiecesSpritePath[] = "assets/pieces-spritesheet.png";

constexpr inline int SquareSize = 128;
constexpr inline ::Vector2 BoardPos = {88, 88};

constexpr ::Rectangle UIPanelRect{
    .x = 1200, //
    .y = 88,
    .width = WindowWidth - 1200 - 88,
    .height = 1024
};

} // namespace ches::config
