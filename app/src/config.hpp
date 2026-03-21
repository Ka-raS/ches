#pragma once
#include <filesystem>
#include <raylib.h>

namespace ches::config {

inline constexpr int WINDOW_WIDTH = 800;
inline constexpr int WINDOW_HEIGHT = 600;
inline constexpr const char *WINDOW_TITLE = "Ches";
inline constexpr Color BACKGROUND_COLOR = {48, 46, 43, 255};

namespace paths {

inline const std::filesystem::path ASSETS = "assets";
inline const std::filesystem::path WHITE_PAWN_PNG   = ASSETS / "white_pawn.png";
inline const std::filesystem::path WHITE_KNIGHT_PNG = ASSETS / "white_knight.png";
inline const std::filesystem::path WHITE_BISHOP_PNG = ASSETS / "white_bishop.png";
inline const std::filesystem::path WHITE_ROOK_PNG   = ASSETS / "white_rook.png";
inline const std::filesystem::path WHITE_QUEEN_PNG  = ASSETS / "white_queen.png";
inline const std::filesystem::path WHITE_KING_PNG   = ASSETS / "white_king.png";
inline const std::filesystem::path BLACK_PAWN_PNG   = ASSETS / "black_pawn.png";
inline const std::filesystem::path BLACK_KNIGHT_PNG = ASSETS / "black_knight.png";
inline const std::filesystem::path BLACK_BISHOP_PNG = ASSETS / "black_bishop.png";
inline const std::filesystem::path BLACK_ROOK_PNG   = ASSETS / "black_rook.png";
inline const std::filesystem::path BLACK_QUEEN_PNG  = ASSETS / "black_queen.png";
inline const std::filesystem::path BLACK_KING_PNG   = ASSETS / "black_king.png";

} // namespace paths

} // namespace ches::config
