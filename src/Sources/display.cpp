#include "../Includes/display.hpp"

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
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            Graphics graphics(hdc);
        
            graphics.Clear(Color(30, 30, 30));
        
            SolidBrush blackBrush(Color(0, 0, 0));
            SolidBrush greenBrush(Color(0, 255, 0));
            Pen borderPen(Color(180, 180, 180), 2);
        
            graphics.DrawRectangle(&borderPen,
                CORNER_OFFSET - 2, CORNER_OFFSET - 2,
                WIDTH * PIXEL + 3, HEIGHT * PIXEL + 3);
        
            graphics.FillRectangle(&blackBrush, CORNER_OFFSET, CORNER_OFFSET, WIDTH * PIXEL, HEIGHT * PIXEL);
        
            const auto& screen = Display::getInstance().getScreen();
            for (int i = 0; i < WIDTH * HEIGHT; ++i) {
                if (screen[i]) {
                    int x = (i % WIDTH) * PIXEL + CORNER_OFFSET;
                    int y = (i / WIDTH) * PIXEL + CORNER_OFFSET;
                    graphics.FillRectangle(&greenBrush, x, y, PIXEL, PIXEL);
                }
            }
        
            FontFamily fontFamily(L"Consolas");
            Font font(&fontFamily, 14, FontStyleRegular, UnitPixel);
            SolidBrush textBrush(Color(220, 220, 220));
            graphics.DrawString(L"CHIP-8 Emulator", -1, &font, PointF(CORNER_OFFSET, CORNER_OFFSET + HEIGHT * PIXEL + 10), &textBrush);
        
            EndPaint(hwnd, &ps);
            return 0;
        }
        

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool Display::initWindow(HINSTANCE hInstance, int nCmdShow) {
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

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

void Display::setPixel(int x, int y, bool value) {
    int pos = y * WIDTH + x;
    screen[pos] = value;
}

void Display::clear() {
    screen.fill(false);
}

bool Display::togglePixel(int x, int y) {
    int pos = y * WIDTH + x;
    screen[pos] ^= 1;
    return !screen[pos];
}