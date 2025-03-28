#include "../Includes/temp.h"

struct {
    bool display[HEIGHT*WIDTH] = {};
} Screen;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Initialize GDI+
    ULONG_PTR gdiplusToken;
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    // Register window class
    const wchar_t CLASS_NAME[] = L"CHIP8WindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    RECT rect = { 0, 0, WIDTH * PIXEL, HEIGHT * PIXEL };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"CHIP-8 GDI+ Test",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
    
    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    // Main message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        Graphics graphics(hdc);
        SolidBrush blackBrush(Color(0, 0, 0));
        SolidBrush whiteBrush(Color(0 , 255, 255));

        // Clear background
        graphics.FillRectangle(&blackBrush, 0, 0, 640, 320);

        //Activate Pixel
        Screen.display[0 * WIDTH + 0] = true;
        Screen.display[31 * WIDTH + 63] = true;
        Screen.display[10 * WIDTH + 10] = true;
        Screen.display[20 * WIDTH + 20] = true;
        Screen.display[15 * WIDTH + 30] = true;
        Screen.display[17 * WIDTH + 40] = true;
        Screen.display[7 * WIDTH + 50] = true;
        Screen.display[4 * WIDTH + 55] = true;

        // Draw white rectangle
        for (int i = 0; i < WIDTH * HEIGHT; ++i) {
            if (Screen.display[i]) {
                int x = (i % WIDTH) * PIXEL;
                int y = (i / WIDTH) * PIXEL;
                graphics.FillRectangle(&whiteBrush, x, y, PIXEL, PIXEL);
            }
        }

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}