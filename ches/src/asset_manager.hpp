#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <raylib.h>

namespace ches {

// TODO: nuke this
class AssetManager {
  public:
    AssetManager() = default;
    std::shared_ptr<::Font> load_font(const char *path);
    std::shared_ptr<::Image> load_image(const char *path);
    std::shared_ptr<::Texture2D> load_texture(const char *path);

  private:
    std::unordered_map<std::string, std::weak_ptr<::Font>> _fonts;
    std::unordered_map<std::string, std::weak_ptr<::Image>> _images;
    std::unordered_map<std::string, std::weak_ptr<::Texture2D>> _textures;
};

} // namespace ches
