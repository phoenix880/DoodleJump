#include "GameManager.h"
#include <algorithm>
#include <iostream>

GameManager::GameManager(const GameConfig& cfg)
    : config(cfg)
    , resources(ResourceManager::getInstance())
    , mouseX(0)
    , mouseY(0)
    , playerCenterX(0)
    , playerCenterY(0)
    , dirX(0)
    , dirY(0)
    , velocityX(0)
    , velocityY(0)
    , lastShotTime(std::chrono::steady_clock::now())
    , score(0)
    , shift(0.0f)
    , gameOver(false)
    , deltaTime(0)
    , lastFrameTime(std::chrono::steady_clock::now())
    , highestPlatformY(450)
    , lastJumpedPlatformId(-1)
    , platformIdCounter(0)
    , screenWidth(0)
    , screenHeight(0)
{
    try {
        if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok) throw std::runtime_error("Failed to initialize GDI+");
       
        platforms.reserve(GameConstants::INITIAL_PLATFORM_COUNT);
        enemies.reserve(GameConstants::INITIAL_PLATFORM_COUNT / 2);
        projectiles.reserve(10);
        jetpacks.reserve(5);

    }
    catch (const std::exception& e) {
        printf("Error in GameManager constructor: %s\n", e.what());
        cleanup(); throw;
    }
}

GameManager::~GameManager() {
    try { cleanup(); Gdiplus::GdiplusShutdown(gdiplusToken); }
    catch (...) {}
}

void GameManager::PreInit(int& width, int& height, bool& fullscreen) {
    width = config.windowWidth;
    height = config.windowHeight;
    fullscreen = config.fullscreen;
}

bool GameManager::Init() { resetGame(); return true; }

void GameManager::Close() { cleanup(); }

void GameManager::showGameOverScreen() {
    HDC hDC = getHDC();

    RECT rect = { 0, 0, screenWidth, screenHeight };
    FillRect(hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    drawSprite(getGameScoreSprite(), 0, screenHeight / 2 - 10);
    drawSprite(getGameScoreSprite(), 0, screenHeight / 2 - 77);

    drawNumber(score, (screenWidth - 100) / 2, screenHeight / 2 - 50);
    drawNumber(player->getPlatformsCount(), (screenWidth - 100) / 2, screenHeight / 2);
}

bool GameManager::Tick() {
    if (gameOver) {
        showGameOverScreen();
        auto currentTime = std::chrono::steady_clock::now();
        float elapsedSeconds = std::chrono::duration<float>(currentTime - gameOverTime).count();
        if (elapsedSeconds >= 2.0f) { resetGame(); gameOver = false; }
        return false;
    }
    updateDeltaTime();

    for (auto& platform : platforms) {
        platform->update(deltaTime);
        platform->render();
    }

    for (auto& projectile : projectiles) projectile->render();
    for (auto& enemy : enemies) enemy->render();
    
    if (player) {
        player->update(deltaTime);
        player->render();
        drawNumber(player->getPlatformsCount(), 20, 20);
        drawNumber(score, config.windowWidth - 100, 20);
    }

    updateEntities();
    handleCollisions();

    platforms.erase(std::remove_if(platforms.begin(), platforms.end(),
        [](const auto& p) { return !p->isActive(); }), platforms.end());

    while (platforms.size() < GameConstants::INITIAL_PLATFORM_COUNT) spawnPlatform();
    return false;
}

void GameManager::updateDeltaTime() {
    auto currentTime = std::chrono::steady_clock::now();
    deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
    deltaTime = min(deltaTime, 0.016f);
    lastFrameTime = currentTime;
}

void GameManager::spawnPlatform() {
    float x = static_cast<float>(rand() % (config.windowWidth - GameConstants::PLATFORM_WIDTH));
    float y = static_cast<float>(highestPlatformY - GameConstants::MIN_PLATFORM_INTERVAL);

    auto platform = std::make_unique<Platform>(x, y, ++platformIdCounter);
    if (platformIdCounter > GameConstants::INITIAL_PLATFORM_COUNT && !platform->getDisappearing()) {
        if ((rand() % 100) < GameConstants::ENEMY_SPAWN_CHANCE) {
            platform->setHasEnemy(true);
            int enemyType = rand() % 3;
            auto enemy = std::make_unique<Enemy>(0, 0, enemyType);

            float enemyX = x + (GameConstants::PLATFORM_WIDTH - enemy->getWidth()) / 2;
            float enemyY = y - enemy->getHeight();

            enemy->setPosition(enemyX, enemyY);
            enemies.push_back(std::move(enemy));
        }
    }

    platforms.push_back(std::move(platform));
    highestPlatformY = static_cast<int>(y);
}

void GameManager::updateEntities() {
    if (!player) return;
    updatePlayer();
    updatePlatforms();
    updateProjectiles();
    updateEnemies();
}

void GameManager::updatePlayer() {
    player->update(deltaTime);
    player->setLookingUp(cooldown());

    getScreenSize(screenWidth, screenHeight);

    if (player->getY() > screenHeight) { gameOver = true; gameOverTime = std::chrono::steady_clock::now(); return; }

    if (player->getY() < screenHeight / 2) {
        shift = screenHeight / 2 - player->getY();
        player->setPosition(player->getX(), screenHeight / 2);

        for (auto& platform : platforms) platform->setPosition(platform->getX(), platform->getY() + shift);
        for (auto& enemy : enemies) enemy->setPosition(enemy->getX(), enemy->getY() + shift);
        
        highestPlatformY += static_cast<int>(shift);
        score += static_cast<int>(shift * 0.5f);
    }
}

void GameManager::updatePlatforms() {
    for (auto& platform : platforms) platform->update(deltaTime);
    getScreenSize(screenWidth, screenHeight);

    platforms.erase(
        std::remove_if(platforms.begin(), platforms.end(),
            [this](const auto& platform) { return platform->getY() > screenHeight || !platform->isActive(); }),
        platforms.end());

    while (platforms.size() < GameConstants::INITIAL_PLATFORM_COUNT) spawnPlatform();
}

void GameManager::handleCollisions() {
    if (!player) return;

    float playerBottom = player->getY() + GameConstants::PLAYER_HEIGHT;
    float playerLeft = player->getX();
    float playerRight = player->getX() + GameConstants::PLAYER_WIDTH;

    int currentPlatformId = -1;

    for (const auto& platform : platforms) {
        float platformTop = platform->getY();
        float platformBottom = platformTop + GameConstants::PLATFORM_HEIGHT;
        float platformLeft = platform->getX();
        float platformRight = platform->getX() + GameConstants::PLATFORM_WIDTH;

        if (playerRight >= platformLeft && playerLeft <= platformRight) {
            if (playerBottom >= platformTop && playerBottom <= platformTop + 10.0f && player->getVelocityY() > 0) {
                player->setPosition(player->getX(), platformTop - GameConstants::PLAYER_HEIGHT);
                player->setVelocityY(0);
                player->jump();
                currentPlatformId = platform->getId();
                if (currentPlatformId != player->getLastJumpedPlatformId()) { player->updatePlatformCount(); player->setLastJumpedPlatformId(currentPlatformId); }
                break;
            }
        }
    }

    for (const auto& enemy : enemies) {
        if (enemy->isActive() &&
            player->collidesWith(*enemy,
                GameConstants::PLAYER_WIDTH,
                GameConstants::PLAYER_HEIGHT,
                GameConstants::ENEMY_WIDTH,
                GameConstants::ENEMY_HEIGHT)) {
            gameOver = true;
            gameOverTime = std::chrono::steady_clock::now();
            return;
        }
    }
}

void GameManager::drawNumber(int number, int x, int y) {
    std::string numberStr = std::to_string(number);
    int xOffset = 0;
    for (size_t i = 0; i < numberStr.length(); ++i) {
        int digitIndex = numberStr[i] - '0';
        drawSprite(player->getDigit(digitIndex), x + xOffset, y);
        xOffset += 20;
    }
}

void GameManager::cleanup() {
    try {
        if (player) player.reset();
        platforms.clear();
        enemies.clear();
        projectiles.clear();
        jetpacks.clear();
    }
    catch (...) {}
}

void GameManager::resetGame() {
    cleanup();

    score = 0;
    gameOver = false;
    highestPlatformY = config.windowHeight - 65;
    lastJumpedPlatformId = -1;
    platformIdCounter = 0;

    float startPlatformX = config.windowWidth / 2.0f - GameConstants::PLATFORM_WIDTH / 2.0f;
    float startPlatformY = config.windowHeight - 65.0f;
    platforms.push_back(std::make_unique<Platform>(startPlatformX, startPlatformY, ++platformIdCounter));

    player = std::make_unique<Player>(
        startPlatformX + (GameConstants::PLATFORM_WIDTH - GameConstants::PLAYER_WIDTH) / 2.0f,
        startPlatformY - GameConstants::PLAYER_HEIGHT -150.0f,
        config);

    for (int i = 0; i < 10; i++) {
        std::string filePath = "material/" + std::to_string(i) + ".png";
        player->setDigit(i, createSprite(filePath.c_str()));
    }

    for (int i = 1; i < GameConstants::INITIAL_PLATFORM_COUNT; ++i) spawnPlatform();
    enemies.clear();
}

bool GameManager::cooldown() {
    auto currentTime = std::chrono::steady_clock::now();
    float elapsedMs = std::chrono::duration<float, std::milli>(currentTime - lastShotTime).count();
    return elapsedMs < GameConstants::PROJECTILE_COOLDOWN;
}

void GameManager::onMouseMove(int x, int y, int xrelative, int yrelative) { mouseX = x; mouseY = y; }
void GameManager::onMouseButtonClick(FRMouseButton button, bool isReleased) {
    if (!player || isReleased) return;
    if (button == FRMouseButton::LEFT) {
        if (!cooldown()) {
            player->setLookingUp(false);

            playerCenterX = player->getX() + GameConstants::PLAYER_WIDTH / 2;
            playerCenterY = player->getY() + GameConstants::PLAYER_HEIGHT / 2;

            float dx = mouseX - playerCenterX;
            float dy = mouseY - playerCenterY;
            float length = sqrt(dx * dx + dy * dy);

            float angle = atan2(dx, -dy) * 180.0f / 3.14159f;
            angle = max(-90.0f, min(90.0f, angle));

            player->setNoseAngle(angle);

            dirX = dx / length;
            dirY = dy / length;

            velocityX = dirX * GameConstants::PROJECTILE_SPEED;
            velocityY = dirY * GameConstants::PROJECTILE_SPEED;

            auto projectile = std::make_unique<Projectile>(playerCenterX, playerCenterY, atan2(dirY, dirX));

            projectile->setVelocityX(velocityX);
            projectile->setVelocityY(velocityY);
            projectiles.push_back(std::move(projectile));
            lastShotTime = std::chrono::steady_clock::now();
        }
    }
}

void GameManager::onKeyPressed(FRKey k) {
    if (!player) return;
    switch (k) {
    case FRKey::LEFT: player->moveLeft(); break;
    case FRKey::RIGHT: player->moveRight(); break;
    }
}

void GameManager::onKeyReleased(FRKey k) {
    if (!player) return;
    switch (k) {
    case FRKey::LEFT: if (player->getVelocityX() < 0) player->stop(); break;
    case FRKey::RIGHT: if (player->getVelocityX() > 0) player->stop(); break;
    }
}

void GameManager::spawnProjectile(float x, float y, float angle) {
    projectiles.push_back(std::make_unique<Projectile>(x + GameConstants::PLAYER_FULL_WIDTH / 2, y, angle));
}

void GameManager::updateProjectiles() {
    for (auto& projectile : projectiles) projectile->update(deltaTime);
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const auto& p) { return !p->isActive(); }), projectiles.end());
}

void GameManager::spawnEnemy() {
    if (platforms.size() < 2) return;
    if ((rand() % 100) >= GameConstants::ENEMY_SPAWN_CHANCE) return;

    int index = 1 + (rand() % (platforms.size() - 1));
    auto& platform = platforms[index];

    float platformX = platform->getX();
    float platformY = platform->getY();

    for (const auto& enemy : enemies) {
        if (abs(enemy->getY() + enemy->getHeight() - platformY) < 5.0f &&
            abs(enemy->getX() - platformX) < GameConstants::PLATFORM_WIDTH) return;
    }

    int enemyType = rand() % 3;
    auto enemy = std::make_unique<Enemy>(0, 0, enemyType);

    float enemyX = platform->getX() + (GameConstants::PLATFORM_WIDTH - enemy->getWidth()) / 2;
    float enemyY = platform->getY() - enemy->getHeight();

    enemy->setPosition(enemyX, enemyY);
    enemies.push_back(std::move(enemy));
}

void GameManager::updateEnemies() {
    for (auto& enemy : enemies) enemy->update(deltaTime);
    
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const auto& e) { return !e->isActive(); }
        ), enemies.end());

    for (auto& enemy : enemies) {
        for (auto& projectile : projectiles) {
            if (enemy->isActive() && projectile->isActive() &&
                enemy->collidesWith(*projectile,
                    enemy->getWidth(),
                    enemy->getHeight(),
                    GameConstants::PROJECTILE_WIDTH,
                    GameConstants::PROJECTILE_HEIGHT)) {
                enemy->setActive(false);
                projectile->setActive(false);
                score += 100;
            }
        }
    }
}

const char* GameManager::GetTitle() { return "Doodle Jump"; }