#include "../Includes/display.hpp"

int scrollPos = 0;
const int lineHeight = 16;

const int memoryViewX = WIDTH * PIXEL + 40;
const int memoryViewY = CORNER_OFFSET + 36;
const int memoryViewWidth = 132;
const int memoryViewHeight = WINDOW_HEIGHT - 13.5 * CORNER_OFFSET;

const int scrollbarWidth = 6;

bool isDraggingScrollbar = false;
int dragStartY = 0;
int scrollStartPos = 0;

void scrollWheel(WPARAM wParam) {
    int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    int scrollAmount = delta > 0 ? -lineHeight : lineHeight;
    scrollPos = std::min(std::max(scrollPos + scrollAmount, 0),
                        (4096 / 4 - memoryViewHeight / lineHeight) * lineHeight);
}

void BeginScrollbarDrag(HWND hwnd, LPARAM lParam) {
    int mouseX = GET_X_LPARAM(lParam);
    int mouseY = GET_Y_LPARAM(lParam);

    int totalLines = 4096 / 4;
    int visibleLines = memoryViewHeight / lineHeight;
    int scrollbarX = memoryViewX + memoryViewWidth - scrollbarWidth;
    int scrollbarHeight = std::max(20, (visibleLines * memoryViewHeight) / totalLines);
    int scrollbarY = memoryViewY + (scrollPos * (memoryViewHeight - scrollbarHeight)) /
                                   ((totalLines - visibleLines) * lineHeight);

    if (mouseX >= scrollbarX && mouseX <= scrollbarX + scrollbarWidth &&
        mouseY >= scrollbarY && mouseY <= scrollbarY + scrollbarHeight) {
        isDraggingScrollbar = true;
        dragStartY = mouseY;
        scrollStartPos = scrollPos;
        SetCapture(hwnd);
    }
}

void UpdateScrollbarDrag(HWND hwnd, LPARAM lParam) {
    if (isDraggingScrollbar) {
        int mouseY = GET_Y_LPARAM(lParam);
        int deltaY = mouseY - dragStartY;

        int totalLines = 4096 / 4;
        int visibleLines = memoryViewHeight / lineHeight;
        int scrollArea = memoryViewHeight - std::max(20, (visibleLines * memoryViewHeight) / totalLines);

        int newScroll = scrollStartPos + (deltaY * (totalLines - visibleLines) * lineHeight) / scrollArea;
        scrollPos = std::min(std::max(newScroll, 0),
                            (totalLines - visibleLines) * lineHeight);

        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void DrawChip8Screen(Graphics& graphics) {
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
}

void DrawTitle(Graphics& graphics) {
    FontFamily fontFamily(L"Consolas");
    Font font(&fontFamily, 14, FontStyleRegular, UnitPixel);
    SolidBrush textBrush(Color(220, 220, 220));
    graphics.DrawString(L"CHIP-8 Emulator", -1, &font,
        PointF(CORNER_OFFSET, CORNER_OFFSET + HEIGHT * PIXEL + 10), &textBrush);
}

void DrawMemoryPanel(Graphics& graphics, const Memory* memory) {
    if (!memory) return;

    FontFamily fontFamily(L"Consolas");
    Font monoFont(&fontFamily, 12, FontStyleRegular, UnitPixel);
    SolidBrush memBrush(Color(200, 200, 200));
    Pen memBorderPen(Color(120, 120, 120), 1);
    SolidBrush memBgBrush(Color(20, 20, 20));

    graphics.FillRectangle(&memBgBrush, memoryViewX, memoryViewY, memoryViewWidth, memoryViewHeight);
    graphics.DrawRectangle(&memBorderPen, memoryViewX, memoryViewY, memoryViewWidth, memoryViewHeight);

    const int memStartX = memoryViewX + 10;
    const int memStartY = memoryViewY + 10;
    const int maxLines = memoryViewHeight / lineHeight;

    for (int i = 0; i < maxLines; ++i) {
        int memIndex = (scrollPos / lineHeight) + i;
        if (memIndex * 4 >= 4096) break;

        std::wstringstream wss;
        wss << std::hex << std::uppercase << std::setw(3) << std::setfill(L'0') << memIndex * 4 << L": ";
        for (int j = 0; j < 4; ++j) {
            wss << std::setw(2) << std::setfill(L'0') << (int)memory->read(memIndex * 4 + j) << L" ";
        }

        graphics.DrawString(wss.str().c_str(), -1, &monoFont,
            PointF(memStartX, static_cast<REAL>(memStartY + i * lineHeight)), &memBrush);
    }
}

void DrawScrollbar(Graphics& graphics) {
    int scrollbarX = memoryViewX + memoryViewWidth - scrollbarWidth;
    int totalLines = 4096 / 4;
    int visibleLines = memoryViewHeight / lineHeight;

    int scrollbarHeight, scrollbarY;
    if (totalLines <= visibleLines) {
        scrollbarHeight = memoryViewHeight;
        scrollbarY = memoryViewY;
    } else {
        scrollbarHeight = std::max(20, (visibleLines * memoryViewHeight) / totalLines);
        scrollbarY = memoryViewY + (scrollPos * (memoryViewHeight - scrollbarHeight)) /
                                   ((totalLines - visibleLines) * lineHeight);
    }

    SolidBrush scrollbarTrack(Color(40, 40, 40));
    SolidBrush scrollbarThumb(Color(150, 150, 150));

    graphics.FillRectangle(&scrollbarTrack, scrollbarX, memoryViewY, scrollbarWidth, memoryViewHeight);
    graphics.FillRectangle(&scrollbarThumb, scrollbarX, scrollbarY, scrollbarWidth, scrollbarHeight);
}

void DrawMemoryHeader(Graphics& graphics) {
    FontFamily fontFamily(L"Consolas");
    Font font(&fontFamily, 14, FontStyleRegular, UnitPixel);
    SolidBrush headerBrush(Color(255, 200, 50));
    graphics.DrawString(L"MEMORY", -1, &font, PointF(memoryViewX, memoryViewY - 24), &headerBrush);
}

void Render(Graphics& graphics) {
    DrawChip8Screen(graphics);
    DrawTitle(graphics);
    DrawMemoryPanel(graphics, Display::getInstance().getMemory());
    DrawScrollbar(graphics);
    DrawMemoryHeader(graphics);
}

void EndDoubleBuffering(HDC hdc, PAINTSTRUCT& ps, HDC memDC, HBITMAP oldBitmap, HBITMAP memBitmap) {
    BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);

    EndPaint(WindowFromDC(hdc), &ps);
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_MOUSEWHEEL:
    {
        scrollWheel(wParam);
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        BeginScrollbarDrag(hwnd,lParam);
        return 0;
    }

    case WM_LBUTTONUP:
        isDraggingScrollbar = false;
        ReleaseCapture();
        return 0;

    case WM_MOUSEMOVE:
    {
        UpdateScrollbarDrag(hwnd, lParam);
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

        Graphics graphics(memDC);
        graphics.Clear(Color(30, 30, 30));

        Render(graphics);

        EndDoubleBuffering(hdc, ps, memDC, oldBitmap, memBitmap);

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
    wc.style = CS_HREDRAW | CS_VREDRAW;

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
        return false;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    return true;
}

void Display::setMemoryPointer(Memory* mem) {
    this->memory = mem;
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
