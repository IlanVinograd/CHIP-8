#ifndef _TEMP_H_
#define _TEMP_H_

#define UNICODE
#define _UNICODE

#define WIDTH 64
#define HEIGHT 32
#define PIXEL 10

#include <iostream>
#include <algorithm>
#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

// Forward declaration
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif /* _TEMP_H_ */