#include "GameManager.h"
#include <windows.h>
#include <vector>
#include <string>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS")

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
#ifdef _DEBUG
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONIN$", "r", stdin);
    printf("Debug console initialized\n");
#endif

    GameConfig config;

    int argc = 0;
    LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argvW && argc > 1) {
        std::vector<std::string> args;
        std::vector<char*> argv(argc);

        for (int i = 0; i < argc; ++i) {
            int size = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, nullptr, 0, nullptr, nullptr);
            std::vector<char> buffer(size);
            WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, buffer.data(), size, nullptr, nullptr);
            args.push_back(buffer.data());
            argv[i] = const_cast<char*>(args.back().c_str());
        }

        config = GameConfig::parseCommandLine(argc, argv.data());
        LocalFree(argvW);
    }

#ifdef _DEBUG
    printf("Window size: %dx%d\n", config.windowWidth, config.windowHeight);
    printf("Fullscreen: %s\n", config.fullscreen ? "true" : "false");
    printf("Player speed: %f\n", config.playerSpeed);
#endif

    try {
        std::unique_ptr<GameManager> game = std::make_unique<GameManager>(config);

#ifdef _DEBUG
        printf("Game instance created\n");
#endif

        int result = run(game.get());

#ifdef _DEBUG
        printf("Game finished with result: %d\n", result);
#endif

        return result;
    }
    catch (const std::exception& e) {
#ifdef _DEBUG
        printf("Error: %s\n", e.what());
#endif
        return -1;
    }

#ifdef _DEBUG
    if (dummy) fclose(dummy);
    FreeConsole();
#endif
}