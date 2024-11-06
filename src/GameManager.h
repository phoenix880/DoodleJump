#pragma once
#include "Entity.h"
#include "GameConfig.h"
#include "ResourceManager.h"
#include <vector>
#include <memory>
#include <chrono>

class GameManager : public Framework {
private:
    GameConfig config;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Platform>> platforms;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    std::vector<std::unique_ptr<Jetpack>> jetpacks;
    int screenWidth, screenHeight;
    int mouseX;
    int mouseY;
    float playerCenterX = 0.0f;
    float playerCenterY = 0.0f;
    float dirX = 0.0f;
    float dirY = 0.0f;
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    std::chrono::steady_clock::time_point gameOverTime;
    std::chrono::steady_clock::time_point lastShotTime;
    int score;
    float shift;
    bool gameOver;
    float deltaTime;
    std::chrono::steady_clock::time_point lastFrameTime;
    ResourceManager& resources;
    int highestPlatformY;
    int lastJumpedPlatformId;
    int platformIdCounter;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

public:
    explicit GameManager(const GameConfig& cfg);
    ~GameManager() override;

    void PreInit(int& width, int& height, bool& fullscreen) override;
    bool Init() override;
    void Close() override;
    void showGameOverScreen();
    bool Tick() override;
    void onMouseMove(int x, int y, int xrelative, int yrelative) override;
    void onMouseButtonClick(FRMouseButton button, bool isReleased) override;
    void onKeyPressed(FRKey k) override;
    void onKeyReleased(FRKey k) override;
    const char* GetTitle() override;

private:
    void updateDeltaTime();
    void updateEntities();
    void updatePlayer();
    void updatePlatforms();
    void updateEnemies();
    void spawnPlatform();
    void spawnEnemy();
    void handleCollisions();
    void cleanup();
    void resetGame();
    void drawNumber(int number, int x, int y);
    void updateProjectiles();
    bool cooldown();
    void spawnProjectile(float x, float y, float angle);
};