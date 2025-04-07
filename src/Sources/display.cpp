#include "../Includes/display.hpp"

Display* Display::instance = nullptr;

LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_DESTROY: 
        {
            PostQuitMessage(0);
            return 0;
        }

    case WM_PAINT:
        {
            Display* display = Display::instance;
            if (!display) break;

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            Graphics graphics(hdc);
            SolidBrush blackBrush(Color(0, 0, 0));
            SolidBrush whiteBrush(Color(0, 255, 0));

            graphics.FillRectangle(&blackBrush, CORNER_OFFSET, CORNER_OFFSET, WIDTH * PIXEL, HEIGHT * PIXEL);

            const auto& screen = display->getScreen();
            for (int i = 0; i < WIDTH * HEIGHT; ++i) {
                if (screen[i]) {
                    int x = (i % WIDTH) * PIXEL + CORNER_OFFSET;
                    int y = (i / WIDTH) * PIXEL + CORNER_OFFSET;
                    graphics.FillRectangle(&whiteBrush, x, y, PIXEL, PIXEL);
                }
            }
        
            EndPaint(hwnd, &ps);
        }
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool Display::initWindow(HINSTANCE hInstance, int nCmdShow) {
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    Display::instance = this;

    const wchar_t CLASS_NAME[] = L"Display Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WinProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"CHIP-8 Emulator",
        WS_OVERLAPPEDWINDOW, // Window style

        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, // Size and position

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,
        NULL        // Additional application data
    );

    if (!hwnd)
    {
        return false;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    hdc = GetDC(hwnd);
    
    return true;
}

const std::array<bool, WIDTH * HEIGHT>& Display::getScreen() const {
     return screen; 
}