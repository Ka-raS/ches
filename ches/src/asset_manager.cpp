#include "asset_manager.hpp"

namespace ches {

AssetManager::~AssetManager() {
    for (const auto &[path, weakTexture] : _textures) {
        if (std::shared_ptr<const Texture2D> texture = weakTexture.lock()) {
            UnloadTexture(*texture);
        }
    }
}

std::shared_ptr<const Texture2D> AssetManager::loadTexture(const std::filesystem::path &path) {
    if (std::weak_ptr<const Texture2D> weakTexture = _textures[path]; !weakTexture.expired()) {
        return weakTexture.lock();
    }

    std::shared_ptr<const Texture2D> texture = std::make_shared<const Texture2D>(LoadTexture(path.string().c_str()));
    _textures[path] = texture;
    return texture;
}

} // namespace ches
