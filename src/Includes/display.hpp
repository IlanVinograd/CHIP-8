#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "../Includes/common.hpp"

#define WIDTH 64
#define HEIGHT 32
#define PIXEL 10

#define WINDOW_WIDTH 940
#define WINDOW_HEIGHT 420
#define CORNER_OFFSET 10

LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
class Display{
public:
    Display(const Display&) = delete;
    Display& operator=(const Display&) = delete;

    static Display& getInstance() {
		static Display instance;
		return instance;
	}

    bool initWindow(HINSTANCE hInstance, int nCmdShow);
    const std::array<bool, WIDTH * HEIGHT>& getScreen() const;
    void setPixel(int x, int y, bool value = true);
    bool togglePixel(int x, int y);
    void clear();

    ~Display() {
        if (gdiplusToken != 0) {
            GdiplusShutdown(gdiplusToken);
        }
    }

private:
    Display() {}

    HWND hwnd = nullptr;    
    HDC hdc = nullptr;
    std::array<bool, WIDTH * HEIGHT> screen;

    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput;
};

#endif /* _DISPLAY_H_ */