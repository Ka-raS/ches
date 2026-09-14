#include "assets.hpp"
#include "config.hpp"

namespace ches {

namespace {

::Texture2D make_placeholder_pieces_sprite() {
    const ::Image image = ::GenImageChecked(
        config::SquareSize * 12, config::SquareSize, config::SquareSize / 8, config::SquareSize / 8, ::MAGENTA, ::BLACK
    );
    const ::Texture2D texture = ::LoadTextureFromImage(image);
    ::UnloadImage(image);
    return texture;
}

} // namespace

Assets::Assets() :
    _font{::LoadFontEx(config::FontPath, config::FontSize, nullptr, 0)},
    _icon{::LoadImage(config::IconPath)},
    _pieces_sprite{::LoadTexture(config::PiecesSpritePath)} {

    if (!::IsFontValid(_font)) {
        _font = ::GetFontDefault();
    }
    if (!::IsTextureValid(_pieces_sprite)) {
        _pieces_sprite = make_placeholder_pieces_sprite();
    }

    ::SetTextureFilter(_font.texture, ::TEXTURE_FILTER_BILINEAR);
    ::SetTextureFilter(_pieces_sprite, ::TEXTURE_FILTER_BILINEAR);
}

Assets::~Assets() {
    ::UnloadFont(_font);
    ::UnloadImage(_icon);
    ::UnloadTexture(_pieces_sprite);
}

const ::Font &Assets::font() const {
    return _font;
}

const ::Image &Assets::icon() const {
    return _icon;
}

const ::Texture2D &Assets::pieces_sprite() const {
    return _pieces_sprite;
}

} // namespace ches
