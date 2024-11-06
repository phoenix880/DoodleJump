#include "GameConfig.h"
#include <cstring>

GameConfig GameConfig::parseCommandLine(int argc, char* argv[]) {
    GameConfig config;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) config.windowWidth = atoi(argv[++i]);
        else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc) config.windowHeight = atoi(argv[++i]);
        else if (strcmp(argv[i], "--fullscreen") == 0) config.fullscreen = true;
        else if (strcmp(argv[i], "--difficulty") == 0 && i + 1 < argc) config.difficulty = static_cast<float>(atof(argv[++i]));
        else if (strcmp(argv[i], "--speed") == 0 && i + 1 < argc) config.playerSpeed = static_cast<float>(atof(argv[++i]));
        else if (strcmp(argv[i], "--debug") == 0) config.debugMode = true;
    }
    return config;
}