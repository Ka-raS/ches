#pragma once
#include <filesystem>
#include <unordered_map>
#include <raylib.h>

namespace ches {

class AssetManager {
  public:
    AssetManager() = default;
    ~AssetManager();
    std::shared_ptr<const Texture2D> loadTexture(const std::filesystem::path &path);

  private:
    std::unordered_map<std::filesystem::path, std::weak_ptr<const Texture2D>> _textures;
};

} // namespace ches
