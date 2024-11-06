#pragma once
#include "framework.h"
#include <vector>
#include <memory>

class ResourceManager {
private:
    std::vector<std::unique_ptr<Sprite>> sprites;
    static ResourceManager* instance;

    ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

public:
    static ResourceManager& getInstance();
    Sprite* loadSprite(const char* path);
    void cleanup();
    ~ResourceManager();
};