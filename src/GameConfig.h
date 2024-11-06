#pragma once
#include <string>

namespace GameConstants {
    constexpr int SCREEN_WIDTH = 600;
    constexpr int SCREEN_HEIGHT = 800;

    constexpr float GRAVITY = 600.0f;
    constexpr float JUMP_FORCE = -500.0f;
    constexpr float BASE_PLAYER_SPEED = 100.0f;
    constexpr float MAX_FALL_SPEED = 400.0f;

    constexpr int PLAYER_WIDTH = 74;
    constexpr int PLAYER_FULL_WIDTH = 124;
    constexpr int PLAYER_HEIGHT = 120;

    constexpr int PLATFORM_WIDTH = 114;
    constexpr int PLATFORM_HEIGHT = 30;
    constexpr int MIN_PLATFORM_INTERVAL = 120;
    constexpr int MAX_PLATFORM_INTERVAL = 210;
    constexpr float PLATFORM_FADE_SPEED = 0.06f;
    constexpr int INITIAL_PLATFORM_COUNT = 10;
    constexpr int PLATFORM_DISAPPEARING_CHANCE = 15;

    constexpr int ENEMY_WIDTH = 50;
    constexpr int ENEMY_HEIGHT = 50;
    constexpr int ENEMY_SPAWN_CHANCE = 10; // %

    constexpr float PROJECTILE_SPEED = 1000.0f;
    constexpr int PROJECTILE_WIDTH = 28;
    constexpr int PROJECTILE_HEIGHT = 28;
    constexpr int PROJECTILE_COOLDOWN = 300; // ms

    constexpr int JETPACK_DURATION = 4000; // ms
    constexpr float JETPACK_SPEED = 400.0f;
    constexpr int JETPACK_WIDTH = 40;
    constexpr int JETPACK_HEIGHT = 40;
    constexpr int JETPACK_SPAWN_CHANCE = 5; // %
}

struct GameConfig {
    int windowWidth = GameConstants::SCREEN_WIDTH;
    int windowHeight = GameConstants::SCREEN_HEIGHT;
    bool fullscreen = false;
    float difficulty = 1.0f;
    float playerSpeed = 5.0f;
    bool debugMode = false;

    static GameConfig parseCommandLine(int argc, char* argv[]);
};