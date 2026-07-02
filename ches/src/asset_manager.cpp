#include "asset_manager.hpp"

namespace ches {

std::shared_ptr<::Font> AssetManager::load_font(const char *const path) {
    std::weak_ptr<::Font> &font_weak = _fonts.try_emplace(path).first->second;
    if (!font_weak.expired()) {
        return font_weak.lock();
    }

    std::shared_ptr<::Font> font{new ::Font{::LoadFont(path)}, [](::Font *const font) {
        ::UnloadFont(*font);
        delete font;
    }};
    ::SetTextureFilter(font->texture, TEXTURE_FILTER_BILINEAR);

    font_weak = font;
    return font;
}

std::shared_ptr<::Image> AssetManager::load_image(const char *const path) {
    std::weak_ptr<::Image> &image_weak = _images.try_emplace(path).first->second;
    if (!image_weak.expired()) {
        return image_weak.lock();
    }

    std::shared_ptr<::Image> image{new ::Image{::LoadImage(path)}, [](::Image *const image) {
        ::UnloadImage(*image);
        ::TraceLog(LOG_WARNING, "unloaded");
        delete image;
    }};
    image_weak = image;
    return image;
}

std::shared_ptr<::Texture2D> AssetManager::load_texture(const char *const path) {
    std::weak_ptr<::Texture2D> &texture_weak = _textures.try_emplace(path).first->second;
    if (!texture_weak.expired()) {
        return texture_weak.lock();
    }

    std::shared_ptr<::Texture2D> texture{new ::Texture2D{::LoadTexture(path)}, [](::Texture2D *const texture) {
        ::UnloadTexture(*texture);
        delete texture;
    }};
    ::SetTextureFilter(*texture, TEXTURE_FILTER_BILINEAR);

    texture_weak = texture;
    return texture;
}

} // namespace ches
