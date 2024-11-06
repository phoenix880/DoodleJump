#define FRAMEWORK_PROJECT
#include "framework.h"
#include <chrono>
#include <stdexcept>

namespace {
    HWND hWnd = nullptr;
    HDC hDC = nullptr;   
    HBITMAP hBackBuffer = nullptr;
    HDC hBackDC = nullptr;
    int screenWidth = 800;
    int screenHeight = 600;
    std::chrono::steady_clock::time_point startTime;
    Framework* currentFramework = nullptr;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    int lastMouseX = 0;
    int lastMouseY = 0;
    Sprite* backgroundSprite = nullptr;
    Sprite* scoreSprite = nullptr;

    // Conversion char* into wchar_t*
    std::wstring convertToWideString(const char* str) {
        if (!str) return L"";
        int size = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
        std::wstring wstr(size - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], size);
        return wstr;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE: PostQuitMessage(0); return 0;
    case WM_MOUSEMOVE:
        if (currentFramework) {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int relX = x - lastMouseX;
            int relY = y - lastMouseY;
            currentFramework->onMouseMove(x, y, relX, relY);
            lastMouseX = x;
            lastMouseY = y;
        }
        return 0;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP: if (currentFramework) currentFramework->onMouseButtonClick(FRMouseButton::LEFT, msg == WM_LBUTTONUP); return 0;
    case WM_KEYDOWN:
        if (currentFramework) {
            FRKey key;
            switch (wParam) {
            case VK_LEFT: key = FRKey::LEFT; break;
            case VK_RIGHT: key = FRKey::RIGHT; break;
            case VK_UP: key = FRKey::UP; break;
            case VK_DOWN: key = FRKey::DOWN; break;
            default: return 0;
            }
            currentFramework->onKeyPressed(key);
        } return 0;
    case WM_KEYUP:
        if (currentFramework) {
            FRKey key;
            switch (wParam) {
            case VK_LEFT: key = FRKey::LEFT; break;
            case VK_RIGHT: key = FRKey::RIGHT; break;
            case VK_UP: key = FRKey::UP; break;
            case VK_DOWN: key = FRKey::DOWN; break;
            default: return 0;
            } currentFramework->onKeyReleased(key);
        } return 0;
    } return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool initWindow(const char* title, bool fullscreen) {
    std::wstring wideClassName = L"FrameworkClass";
    std::wstring wideTitle = convertToWideString(title);

    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = wideClassName.c_str();

    if (!RegisterClassExW(&wc)) return false;

    DWORD style = WS_OVERLAPPEDWINDOW;
    if (fullscreen) {
        style = WS_POPUP | WS_VISIBLE;
        screenWidth = GetSystemMetrics(SM_CXSCREEN);
        screenHeight = GetSystemMetrics(SM_CYSCREEN);
    }

    RECT rect = { 0, 0, screenWidth, screenHeight };
    AdjustWindowRect(&rect, style, FALSE);

    hWnd = CreateWindowW(
        wideClassName.c_str(),
        wideTitle.c_str(),
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (!hWnd) return false;
    
    hDC = GetDC(hWnd);
    hBackDC = CreateCompatibleDC(hDC);
    hBackBuffer = CreateCompatibleBitmap(hDC, screenWidth, screenHeight);
    SelectObject(hBackDC, hBackBuffer);
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    return true;
}

#ifdef __cplusplus
extern "C" {
#endif
    FRAMEWORK_API HDC getHDC() { return hDC; }
    FRAMEWORK_API Sprite* getGameScoreSprite() { return scoreSprite; }

    FRAMEWORK_API void setBackgroundSprite(const char* path) { backgroundSprite = createSprite(path); }

    FRAMEWORK_API void setScoreSprite(const char* path) { scoreSprite = createSprite(path); }

    FRAMEWORK_API Sprite* createSprite(const char* path) {
        if (!path) return nullptr;

        std::wstring widePath = convertToWideString(path);
        Gdiplus::Bitmap* bitmap = nullptr;

        try {
            bitmap = new Gdiplus::Bitmap(widePath.c_str());
            if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok) { delete bitmap; return nullptr; }
            return new Sprite(bitmap);
        }
        catch (const std::exception& e) {
#ifdef _DEBUG
            printf("Exception while loading sprite: %s - %s\n", path, e.what());
#endif
            delete bitmap;
            return nullptr;
        }
    }

    FRAMEWORK_API void drawSprite(Sprite* sprite, int x, int y) {
        if (!sprite || !sprite->getBitmap()) return;
        Gdiplus::Graphics graphics(hBackDC);
        graphics.DrawImage(
            sprite->getBitmap(),
            x, y,
            sprite->getWidth(),
            sprite->getHeight()
        );
    }

    FRAMEWORK_API void drawSpriteRotated(Sprite* sprite, int x, int y, float angle) {
        if (!sprite || !sprite->getBitmap()) return;

        Gdiplus::Graphics graphics(hBackDC);

        Gdiplus::Matrix originalMatrix;
        graphics.GetTransform(&originalMatrix);

        Gdiplus::Matrix rotationMatrix;
        rotationMatrix.RotateAt(
            angle,
            Gdiplus::PointF(
                static_cast<float>(x + sprite->getWidth() / 2),
                static_cast<float>(y + sprite->getHeight() / 2)
            )
        );

        graphics.SetTransform(&rotationMatrix);

        graphics.DrawImage(
            sprite->getBitmap(),
            x, y,
            sprite->getWidth(),
            sprite->getHeight()
        );
        graphics.SetTransform(&originalMatrix);
    }

    FRAMEWORK_API void getSpriteSize(Sprite* sprite, int& width, int& height) {
        if (!sprite) return; width = sprite->getWidth(); height = sprite->getHeight();
        }

    FRAMEWORK_API void setSpriteSize(Sprite* sprite, int width, int height) {
        if (!sprite) return; sprite->setSize(width, height);
    }

    FRAMEWORK_API void destroySprite(Sprite* sprite) { delete sprite; }

    FRAMEWORK_API void drawTestBackground() {
        if (backgroundSprite && scoreSprite) {
            drawSprite(backgroundSprite, 0, 0);
            drawSprite(scoreSprite, 0, 0);
        }
        else {
            RECT rect = { 0, 0, screenWidth, screenHeight };
            FillRect(hBackDC, &rect, (HBRUSH)GetStockObject(GRAY_BRUSH));
        }
    }

    FRAMEWORK_API void getScreenSize(int& width, int& height) {
        width = screenWidth; height = screenHeight;
    }

    FRAMEWORK_API unsigned int getTickCount() {
        auto now = std::chrono::steady_clock::now();
        return static_cast<unsigned int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count()
            );
    }

    FRAMEWORK_API void showCursor(bool show) { ::ShowCursor(show); }

    FRAMEWORK_API int run(Framework* framework) {
        if (!framework) return -1;
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
        currentFramework = framework;

        int width, height;
        bool fullscreen;
        framework->PreInit(width, height, fullscreen);
        screenWidth = width;
        screenHeight = height;

        if (!initWindow(framework->GetTitle(), fullscreen)) return -1;
        startTime = std::chrono::steady_clock::now();
        if (!framework->Init()) return -1;

        setBackgroundSprite("material/backdrop.jpg");
        setScoreSprite("material/score.png");

        MSG msg;
        bool running = true;

        while (running) {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) { running = false; break; }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if (running) {
                drawTestBackground();
                if (framework->Tick()) running = false;
                BitBlt(hDC, 0, 0, screenWidth, screenHeight,
                    hBackDC, 0, 0, SRCCOPY);
            }
        }

        framework->Close();
        currentFramework = nullptr;

        DeleteDC(hBackDC);
        DeleteObject(hBackBuffer);
        ReleaseDC(hWnd, hDC);
        DestroyWindow(hWnd);

        Gdiplus::GdiplusShutdown(gdiplusToken);
        return 0;
    }

#ifdef __cplusplus
}
#endif