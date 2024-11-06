#include "Entity.h"
Platform::Platform(float x, float y, int platformId)
    : Entity(x, y)
    , id(platformId)
    , opacity(1.0f)
    , isDisappearing(false)
    , hasEnemy(false)
    , isCheckedForDisappearing(false)
    , currentSprite(nullptr)
{
    getScreenSize(screenWidth, screenHeight);

    sprites["100"] = std::unique_ptr<Sprite>(createSprite("material/100.png"));
    sprites["75"] = std::unique_ptr<Sprite>(createSprite("material/75.png"));
    sprites["25"] = std::unique_ptr<Sprite>(createSprite("material/25.png"));
    currentSprite = sprites["100"].get();
}

void Platform::update(float deltaTime) {
    if (!isCheckedForDisappearing && !hasEnemy && y <= screenHeight && y >= -10) {
        isCheckedForDisappearing = true;
        isDisappearing = (rand() % 100 < GameConstants::PLATFORM_DISAPPEARING_CHANCE);
    }

    if (isDisappearing) {
        opacity -= GameConstants::PLATFORM_FADE_SPEED * deltaTime;

        if (opacity > 0.75f) currentSprite = sprites["100"].get();
        else if (opacity > 0.25f) currentSprite = sprites["75"].get();
        else if (opacity > 0) currentSprite = sprites["25"].get();
        else active = false;
    }
}

void Platform::render() { if (currentSprite && active) drawSprite(currentSprite, static_cast<int>(x), static_cast<int>(y)); }

Enemy::Enemy(float x, float y, int enemyType)
    : Entity(x, y)
    , type(enemyType)
    , width(0)
    , height(0)
{
    switch (type) {
    case 0: width = 152; height = 111; break;
    case 1: width = 71;  height = 94;  break;
    case 2: width = 129; height = 172; break;
    }

    std::string spritePath = "material/monst" + std::to_string(type + 1) + ".png";
    sprite = std::unique_ptr<Sprite>(createSprite(spritePath.c_str()));
}

void Enemy::update(float deltaTime) { if (!active) return; }

void Enemy::render() {
    if (!active || !sprite) return;
    drawSprite(sprite.get(), static_cast<int>(x), static_cast<int>(y));
}

Player::Player(float x, float y, const GameConfig& cfg)
    : Entity(x, y)
    , gravity(GameConstants::GRAVITY)
    , config(cfg)
    , currentSprite(nullptr)
{
    velocityY = 50;

    sprites["leftup"] = std::unique_ptr<Sprite>(createSprite("material/leftup.png"));
    sprites["leftdown"] = std::unique_ptr<Sprite>(createSprite("material/leftdown.png"));
    sprites["rightup"] = std::unique_ptr<Sprite>(createSprite("material/rightup.png"));
    sprites["rightdown"] = std::unique_ptr<Sprite>(createSprite("material/rightdown.png"));
    sprites["upup"] = std::unique_ptr<Sprite>(createSprite("material/upup.png"));
    sprites["updown"] = std::unique_ptr<Sprite>(createSprite("material/updown.png"));
    noseSprite.reset(createSprite("material/nose.png"));

    for (int i = 0; i < 10; i++) {
        std::string filePath = "material/" + std::to_string(i) + ".png"; digits[i] = createSprite(filePath.c_str());
    } currentSprite = sprites["rightup"].get();
}

void Player::update(float deltaTime) {   
    if (!hasJetpack) {
        velocityY += gravity * deltaTime;
        velocityY = min(velocityY, GameConstants::MAX_FALL_SPEED);
    }

    y += velocityY * deltaTime;
    x += velocityX * deltaTime;

    getScreenSize(screenWidth, screenHeight);
    if (x < 0) x = 0;
    if (x > screenWidth - GameConstants::PLAYER_WIDTH) x = screenWidth - GameConstants::PLAYER_WIDTH;

    std::string spriteKey;
    if (isLookingUp) spriteKey = velocityY > 50.0f ? "updown" : "upup";
    else {
        if (isLookingRight) spriteKey = velocityY > 50.0f ? "rightdown" : "rightup";
        else spriteKey = velocityY > 50.0f ? "leftdown" : "leftup";
    } currentSprite = sprites[spriteKey].get();
}

void Player::render() { 
    if (currentSprite) {
        drawSprite(currentSprite, static_cast<int>(x), static_cast<int>(y));
        if (isLookingUp && noseSprite) {
            float noseX = x + (GameConstants::PLAYER_FULL_WIDTH - 28) / 2 - 3;
            float noseY = y + 3;
            if (currentSprite == sprites["updown"].get()) noseY += 6.0f;
            drawSpriteRotated(noseSprite.get(),
                static_cast<int>(noseX),
                static_cast<int>(noseY),
                noseAngle);
        }
    }
}

void Player::moveLeft() {
    velocityX = -config.playerSpeed * GameConstants::BASE_PLAYER_SPEED;
    isLookingRight = false;
}

void Player::moveRight() {
    velocityX = config.playerSpeed * GameConstants::BASE_PLAYER_SPEED;
    isLookingRight = true;
}

void Player::stop() { velocityX = 0; }

Sprite* Player::getDigit(int index) { return digits[index]; }

Projectile::Projectile(float x, float y, float shootAngle)
    : Entity(x, y)
    , angle(shootAngle) { sprite.reset(createSprite("material/projectile.png")); }

void Projectile::update(float deltaTime) {
    x += velocityX * deltaTime;
    y += velocityY * deltaTime;

    getScreenSize(screenWidth, screenHeight);
    if (x < 0 || x > screenWidth || y < 0 || y > screenHeight) active = false;
}

void Projectile::render() { if (sprite && active) drawSprite(sprite.get(), static_cast<int>(x), static_cast<int>(y)); }