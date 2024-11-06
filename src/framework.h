#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#pragma comment(lib, "gdiplus.lib")

#if defined(_WINDOWS)
#if defined(FRAMEWORK_PROJECT)
#define FRAMEWORK_API __declspec(dllexport)
#else
#define FRAMEWORK_API __declspec(dllimport)
#endif
#else
#define FRAMEWORK_API 
#endif

class Sprite {
private:
    std::unique_ptr<Gdiplus::Bitmap> bitmap;
    int width;
    int height;
public:
    Sprite(Gdiplus::Bitmap* bmp)
        : bitmap(bmp)
        , width(bmp ? bmp->GetWidth() : 0)
        , height(bmp ? bmp->GetHeight() : 0)
    { if (!bmp || bmp->GetLastStatus() != Gdiplus::Ok) throw std::runtime_error("Invalid bitmap in Sprite constructor"); }

    Gdiplus::Bitmap* getBitmap() const { return bitmap.get(); }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    void setSize(int w, int h) { width = w; height = h; }
};

enum class FRKey {
    RIGHT,
    LEFT,
    DOWN,
    UP,
    COUNT
};

enum class FRMouseButton {
    LEFT,
    MIDDLE,
    RIGHT,
    COUNT
};

class Framework {
public:
    virtual void PreInit(int& width, int& height, bool& fullscreen) = 0;
    virtual bool Init() = 0;
    virtual void Close() = 0;
    virtual bool Tick() = 0;
    virtual void onMouseMove(int x, int y, int xrelative, int yrelative) = 0;
    virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) = 0;
    virtual void onKeyPressed(FRKey k) = 0;
    virtual void onKeyReleased(FRKey k) = 0;
    virtual const char* GetTitle() = 0;
    virtual ~Framework() = default;
};

#ifdef __cplusplus
extern "C" {
#endif
    FRAMEWORK_API HDC getHDC();
    FRAMEWORK_API Sprite* getGameScoreSprite();
    FRAMEWORK_API void setBackgroundSprite(const char* path);
    FRAMEWORK_API void setScoreSprite(const char* path);
    FRAMEWORK_API Sprite* createSprite(const char* path);
    FRAMEWORK_API void drawSprite(Sprite* sprite, int x, int y);
    FRAMEWORK_API void drawSpriteRotated(Sprite* sprite, int x, int y, float angle);
    FRAMEWORK_API void getSpriteSize(Sprite* sprite, int& width, int& height);
    FRAMEWORK_API void setSpriteSize(Sprite* sprite, int width, int height);
    FRAMEWORK_API void destroySprite(Sprite* sprite);
    FRAMEWORK_API void drawTestBackground();
    FRAMEWORK_API void getScreenSize(int& width, int& height);
    FRAMEWORK_API unsigned int getTickCount();
    FRAMEWORK_API void showCursor(bool show);
    FRAMEWORK_API int run(Framework* framework);
#ifdef __cplusplus
}
#endif