#include "ResourceManager.h"

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager& ResourceManager::getInstance() {
    if (!instance) instance = new ResourceManager();
    return *instance;
}

Sprite* ResourceManager::loadSprite(const char* path) {
    if (!path) return nullptr;
    auto sprite = createSprite(path);
    if (sprite) {
        sprites.push_back(std::unique_ptr<Sprite>(sprite));
        return sprites.back().get();
    }
    return nullptr;
}

void ResourceManager::cleanup() { sprites.clear(); }

ResourceManager::~ResourceManager() {
    cleanup();
    if (instance == this) instance = nullptr;
}