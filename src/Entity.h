#pragma once
#include "framework.h"
#include "GameConfig.h"
#include <memory>
#include <map>
#include <string>
#include <chrono>

class Entity {
protected:
    int screenWidth = GameConstants::SCREEN_WIDTH;
    int screenHeight = GameConstants::SCREEN_HEIGHT;
    float x, y;
    float velocityX = 0;
    float velocityY = 0;
    bool active = true;
    std::unique_ptr<Sprite> sprite;

public:
    Entity(float startX, float startY) : x(startX), y(startY) {}

    virtual ~Entity() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    bool isActive() const { return active; }
    void setActive(bool value) { active = value; }

    float getX() const { return x; }
    float getY() const { return y; }
    float getVelocityX() const { return velocityX; }
    float getVelocityY() const { return velocityY; }
    void setVelocityX(float newVelocity) { velocityX = newVelocity; }
    void setVelocityY(float newVelocity) { velocityY = newVelocity; }

    void setPosition(float newX, float newY) { x = newX; y = newY; }

    bool collidesWith(const Entity& other, float width1, float height1, float width2, float height2) const { return x < other.x + width2 && x + width1 > other.x && y < other.y + height2 && y + height1 > other.y; }
};

class Platform : public Entity {
private:
    int id;
    bool isDisappearing;
    bool isCheckedForDisappearing;
    bool hasEnemy;
    float opacity;
    std::map<std::string, std::unique_ptr<Sprite>> sprites;
    Sprite* currentSprite;

public:
    Platform(float x, float y, int platformId);
    void update(float deltaTime) override;
    void render() override;
    int getId() const { return id; }
    void startDisappearing() { isDisappearing = true; }
    bool getDisappearing() const { return isDisappearing; }
    void setHasEnemy(bool value) { hasEnemy = value; }
};

class Enemy : public Entity {
private:
    int type;
    int width;
    int height;
public:
    Enemy(float x, float y, int enemyType);
    void update(float deltaTime) override;
    void render() override;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

class Projectile : public Entity {
private:
    float angle;
public:
    Projectile(float x, float y, float shootAngle);
    void update(float deltaTime) override;
    void render() override;
};

class Jetpack : public Entity {
private:
    int duration;
public:
    Jetpack(float x, float y);
    void update(float deltaTime) override;
    void render() override;
    int getRemainingDuration() const { return duration; }
};

class Player : public Entity {
private:
    float gravity;
    bool isLookingRight = true;
    bool isCrouching = false;
    bool canShoot = true;
    std::chrono::steady_clock::time_point lastShotTime;
    bool hasJetpack = false;
    int jetpackTime = 0;
    const GameConfig& config;
    std::map<std::string, std::unique_ptr<Sprite>> sprites;
    std::unique_ptr<Sprite> noseSprite;
    bool isLookingUp = false;
    float noseAngle = 0;
    Sprite* currentSprite;
    int lastJumpedPlatformId = -1;
    int platformsCount = 0;
    Sprite* digits[10];

public:
    Player(float x, float y, const GameConfig& cfg);
    void update(float deltaTime) override;
    void render() override;
    void moveLeft();
    void moveRight();
    void stop();
    void jump() { velocityY = GameConstants::JUMP_FORCE; }
    bool isLookingToRight() const { return isLookingRight; }
    int getPlatformsCount() const { return platformsCount; }
    int getLastJumpedPlatformId() const { return lastJumpedPlatformId; }
    void setLastJumpedPlatformId(int id) { lastJumpedPlatformId = id; }
    void updatePlatformCount() { ++platformsCount; }
    Sprite* getDigit(int index);
    void setDigit(int index, Sprite* sprite) { digits[index] = sprite; }
    void setLookingUp(bool value) { isLookingUp = value; }
    float getNoseAngle() const { return noseAngle; }
    void setNoseAngle(float angle) { noseAngle = angle; }
};