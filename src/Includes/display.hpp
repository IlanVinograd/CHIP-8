#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "../Includes/common.hpp"
#include "../Includes/memory.hpp"

#include <windowsx.h>
#include <algorithm>
#include <sstream>
#include <iomanip>

class Memory;

#define WIDTH 64
#define HEIGHT 32
#define PIXEL 10

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 420
#define CORNER_OFFSET 10

LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
class Display {
public:
    Display(const Display&) = delete;
    Display& operator=(const Display&) = delete;

    static Display& getInstance() {
		static Display instance;
		return instance;
	}

    bool initWindow(HINSTANCE hInstance, int nCmdShow);
    void setMemoryPointer(Memory* mem);
    Memory* getMemory() const { return memory; }

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

    Memory* memory = nullptr;
    HWND hwnd = nullptr;
    std::array<bool, WIDTH * HEIGHT> screen;

    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput;
};

#endif /* _DISPLAY_H_ */