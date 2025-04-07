#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "../Includes/common.hpp"

#define WIDTH 64
#define HEIGHT 32
#define PIXEL 10

class Display{
public:
    bool initWindow(HINSTANCE hInstance, int nCmdShow);

private:
    HWND hwnd = nullptr;    
    HDC hdc = nullptr;
    std::array<bool, WIDTH * HEIGHT> screen;
};

#endif /* _DISPLAY_H_ */