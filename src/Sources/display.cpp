#include "../Includes/display.hpp"

const int lineHeight = 16;

/* Memory */
const int memoryViewX = WIDTH * PIXEL + 40;
const int memoryViewY = CORNER_OFFSET + 36;
const int memoryViewWidth = 132;
const int memoryViewHeight = WINDOW_HEIGHT - 13.5 * CORNER_OFFSET;

int memScrollPos = 0;
bool isDraggingMemScrollbar = false;
int memDragStartY = 0;
int memScrollStartPos = 0;

const int memScrollbarWidth = 6;

/* Stack */
int stackScrollPos = 0;
bool isDraggingStackScrollbar = false;
int stackDragStartY = 0;
int stackScrollStartPos = 0;

const int stackScrollbarWidth = 6;

const int cpuViewX = WIDTH * PIXEL + 180;
const int cpuViewY = CORNER_OFFSET + 36;
const int cpuViewWidth = 76;
const int cpuViewHeight = WINDOW_HEIGHT - 13.5 * CORNER_OFFSET;

/* Registers */
const int regViewX = WIDTH * PIXEL + 265;
const int regViewY = CORNER_OFFSET + 36;
const int regViewWidth = 180;
const int regViewHeight = (WINDOW_HEIGHT - 13.5 * CORNER_OFFSET) / 2 + 4;

/* Special Registers*/
const int sRegViewX = WIDTH * PIXEL + 265;
const int sRegViewY = CORNER_OFFSET + 188;
const int sRegViewWidth = 180;
const int sRegViewHeight = (WINDOW_HEIGHT - 13.5 * CORNER_OFFSET) / 2 - 75;

void memScrollWheel(WPARAM wParam) {
    int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    int scrollAmount = delta > 0 ? -lineHeight : lineHeight;
    memScrollPos = std::min(std::max(memScrollPos + scrollAmount, 0),
                        (4096 / 4 - memoryViewHeight / lineHeight) * lineHeight);
}

void stackScrollWheel(WPARAM wParam) {
    int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    int scrollAmount = delta > 0 ? -lineHeight : lineHeight;
    stackScrollPos = std::min(std::max(stackScrollPos + scrollAmount, 0),
                              (64 - cpuViewHeight / lineHeight) * lineHeight);
}

void BeginMemScrollbarDrag(HWND hwnd, LPARAM lParam) {
    int mouseX = GET_X_LPARAM(lParam);
    int mouseY = GET_Y_LPARAM(lParam);

    int totalLines = 4096 / 4;
    int visibleLines = memoryViewHeight / lineHeight;
    int scrollbarX = memoryViewX + memoryViewWidth - memScrollbarWidth;
    int scrollbarHeight = std::max(20, (visibleLines * memoryViewHeight) / totalLines);
    int scrollbarY = memoryViewY + (memScrollPos * (memoryViewHeight - scrollbarHeight)) /
                                   ((totalLines - visibleLines) * lineHeight);

    if (mouseX >= scrollbarX && mouseX <= scrollbarX + memScrollbarWidth &&
        mouseY >= scrollbarY && mouseY <= scrollbarY + scrollbarHeight) {
        isDraggingMemScrollbar = true;
        memDragStartY = mouseY;
        memScrollStartPos = memScrollPos;
        SetCapture(hwnd);
    }
}

void BeginStackScrollbarDrag(HWND hwnd, LPARAM lParam) {
    int mouseX = GET_X_LPARAM(lParam);
    int mouseY = GET_Y_LPARAM(lParam);

    int totalLines = 64;
    int visibleLines = cpuViewHeight / lineHeight;
    int scrollbarX = cpuViewX + cpuViewWidth - stackScrollbarWidth;
    int scrollbarHeight = std::max(20, (visibleLines * cpuViewHeight) / totalLines);
    int scrollbarY = cpuViewY + (stackScrollPos * (cpuViewHeight - scrollbarHeight)) /
                                 ((totalLines - visibleLines) * lineHeight);

    if (mouseX >= scrollbarX && mouseX <= scrollbarX + stackScrollbarWidth &&
        mouseY >= scrollbarY && mouseY <= scrollbarY + scrollbarHeight) {
        isDraggingStackScrollbar = true;
        stackDragStartY = mouseY;
        stackScrollStartPos = stackScrollPos;
        SetCapture(hwnd);
    }
}

void UpdateMemScrollbarDrag(HWND hwnd, LPARAM lParam) {
    if (isDraggingMemScrollbar) {
        int mouseY = GET_Y_LPARAM(lParam);
        int deltaY = mouseY - memDragStartY;

        int totalLines = 4096 / 4;
        int visibleLines = memoryViewHeight / lineHeight;
        int scrollArea = memoryViewHeight - std::max(20, (visibleLines * memoryViewHeight) / totalLines);

        int newScroll = memScrollStartPos + (deltaY * (totalLines - visibleLines) * lineHeight) / scrollArea;
        memScrollPos = std::min(std::max(newScroll, 0),
                            (totalLines - visibleLines) * lineHeight);

        InvalidateRect(hwnd, NULL, TRUE);
    }
}

void UpdateStackScrollbarDrag(HWND hwnd, LPARAM lParam) {
    if (isDraggingStackScrollbar) {
        int mouseY = GET_Y_LPARAM(lParam);
        int deltaY = mouseY - stackDragStartY;

        int totalLines = 64;
        int visibleLines = cpuViewHeight / lineHeight;
        int scrollArea = cpuViewHeight - std::max(20, (visibleLines * cpuViewHeight) / totalLines);

        int newScroll = stackScrollStartPos + (deltaY * (totalLines - visibleLines) * lineHeight) / scrollArea;

        stackScrollPos = std::min(std::max(newScroll, 0),
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

    const auto& screen = instance.getScreen();
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

    for (int i = 0; i < maxLines; i++) {
        int memIndex = (memScrollPos / lineHeight) + i;
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

void DrawStackPanel(Graphics& graphics, const CPU* cpu) {
    if(!cpu) return;

    FontFamily fontFamily(L"Consolas");
    Font monoFont(&fontFamily, 12, FontStyleRegular, UnitPixel);
    SolidBrush cpuBrush(Color(200, 200, 200));
    Pen cpuBorderPen(Color(120, 120, 120), 1);
    SolidBrush cpuBgBrush(Color(20, 20, 20));

    graphics.FillRectangle(&cpuBgBrush, cpuViewX, cpuViewY, cpuViewWidth, cpuViewHeight);
    graphics.DrawRectangle(&cpuBorderPen, cpuViewX, cpuViewY, cpuViewWidth, cpuViewHeight);

    const int stackStartX = cpuViewX + 10;
    const int stackStartY = cpuViewY + 10;
    const int maxLines = cpuViewHeight / lineHeight;
    
    for (int i = 0; i < maxLines; i++) {
        int stackIndex = (stackScrollPos / lineHeight) + i;
        if (stackIndex >= 64) break;

        std::wstringstream wss;
        wss << L"#" << std::setw(2) << std::setfill(L'0') << stackIndex << L": ";

        try {
            wss << std::hex << std::uppercase
                << std::setw(2) << std::setfill(L'0') << (int)cpu->peek(stackIndex);
        } catch (...) {
            wss << L"--";
        }

        graphics.DrawString(wss.str().c_str(), -1, &monoFont,
            PointF(stackStartX, static_cast<REAL>(stackStartY + i * lineHeight)), &cpuBrush);
    }
}

void DrawRegPanel(Graphics& graphics, const CPU* cpu) {
    if (!cpu) return;

    FontFamily fontFamily(L"Consolas");
    Font monoFont(&fontFamily, 16, FontStyleRegular, UnitPixel);
    SolidBrush cpuBrush(Color(200, 200, 200));
    Pen cpuBorderPen(Color(120, 120, 120), 1);
    SolidBrush cpuBgBrush(Color(20, 20, 20));

    graphics.FillRectangle(&cpuBgBrush, regViewX, regViewY, regViewWidth, regViewHeight);
    graphics.DrawRectangle(&cpuBorderPen, regViewX, regViewY, regViewWidth, regViewHeight);

    const int regStartX = regViewX + 10;
    const int regStartY = regViewY - 8;
    const int lineSpacing = 15.5f;
    const int columnOffset = 90;

    for (int i = 0; i < 8; ++i) {
        std::wstringstream leftWss, rightWss;

        leftWss << L"V" << std::hex << std::uppercase << i << L": ";
        try {
            leftWss << std::setw(2) << std::setfill(L'0') << (int)cpu->getReg(i);
        } catch (...) {
            leftWss << L"--";
        }

        rightWss << L"V" << std::hex << std::uppercase << (i + 8) << L": ";
        try {
            rightWss << std::setw(2) << std::setfill(L'0') << (int)cpu->getReg(i + 8);
        } catch (...) {
            rightWss << L"--";
        }

        graphics.DrawString(leftWss.str().c_str(), -1, &monoFont,
            PointF(regStartX, static_cast<REAL>(regStartY + i * lineHeight) + lineSpacing), &cpuBrush);

        graphics.DrawString(rightWss.str().c_str(), -1, &monoFont,
            PointF(regStartX + columnOffset, static_cast<REAL>(regStartY + i * lineHeight) + lineSpacing), &cpuBrush);
    }
}

void DrawSpecialRegPanel(Graphics& graphics, const CPU* cpu){
    if(!cpu) return;

    FontFamily fontFamily(L"Consolas");
    Font monoFont(&fontFamily, 16, FontStyleRegular, UnitPixel);
    SolidBrush cpuBrush(Color(200, 200, 200));
    Pen cpuBorderPen(Color(120, 120, 120), 1);
    SolidBrush cpuBgBrush(Color(20, 20, 20));

    graphics.FillRectangle(&cpuBgBrush, sRegViewX, sRegViewY, sRegViewWidth, sRegViewHeight);
    graphics.DrawRectangle(&cpuBorderPen, sRegViewX, sRegViewY, sRegViewWidth, sRegViewHeight);

    const int sRegStartX = sRegViewX + 10;
    const int sRegStartY = sRegViewY - 8;

    auto drawString = [&](const std::wstring& reg, auto getter, int Y, int X) {
        std::wstringstream wss;

        using RetType = decltype((cpu->*getter)());
        int width = sizeof(RetType) * 2;

        wss << reg << L": ";
        try {
            wss << std::setw(width) << std::setfill(L'0') << std::hex << std::uppercase
                << static_cast<int>((cpu->*getter)());
        } catch (...) {
            wss << L"--";
        }

        graphics.DrawString(wss.str().c_str(), -1, &monoFont,
            PointF(sRegStartX + X, static_cast<REAL>(sRegStartY + Y * lineHeight) + 15.5f), &cpuBrush);
    };

    drawString(L"PC", &CPU::getPC, 0, 1);
    drawString(L"SP", &CPU::getSP, 1, 1);
    drawString(L"I", &CPU::getI, 2, 10);
    drawString(L"DT", &CPU::getDT, 0, 95);
    drawString(L"ST", &CPU::getST, 1, 95);
}

void DrawMemScrollbar(Graphics& graphics) {
    int scrollbarX = memoryViewX + memoryViewWidth - memScrollbarWidth;
    int totalLines = 4096 / 4;
    int visibleLines = memoryViewHeight / lineHeight;

    int scrollbarHeight, scrollbarY;
    if (totalLines <= visibleLines) {
        scrollbarHeight = memoryViewHeight;
        scrollbarY = memoryViewY;
    } else {
        scrollbarHeight = std::max(20, (visibleLines * memoryViewHeight) / totalLines);
        scrollbarY = memoryViewY + (memScrollPos * (memoryViewHeight - scrollbarHeight)) /
                                   ((totalLines - visibleLines) * lineHeight);
    }

    SolidBrush scrollbarTrack(Color(40, 40, 40));
    SolidBrush scrollbarThumb(Color(150, 150, 150));

    graphics.FillRectangle(&scrollbarTrack, scrollbarX, memoryViewY, memScrollbarWidth, memoryViewHeight);
    graphics.FillRectangle(&scrollbarThumb, scrollbarX, scrollbarY, memScrollbarWidth, scrollbarHeight);
}

void DrawStackScrollbar(Graphics& graphics) {
    int scrollbarX = cpuViewX + cpuViewWidth - stackScrollbarWidth;
    int totalLines = 64;
    int visibleLines = cpuViewHeight / lineHeight;

    int scrollbarHeight, scrollbarY;
    if (totalLines <= visibleLines) {
        scrollbarHeight = cpuViewHeight;
        scrollbarY = cpuViewY;
    } else {
        scrollbarHeight = std::max(20, (visibleLines * cpuViewHeight) / totalLines);
        scrollbarY = cpuViewY + (stackScrollPos * (cpuViewHeight - scrollbarHeight)) /
                                  ((totalLines - visibleLines) * lineHeight);
    }

    SolidBrush scrollbarTrack(Color(40, 40, 40));
    SolidBrush scrollbarThumb(Color(150, 150, 150));

    graphics.FillRectangle(&scrollbarTrack, scrollbarX, cpuViewY, stackScrollbarWidth, cpuViewHeight);
    graphics.FillRectangle(&scrollbarThumb, scrollbarX, scrollbarY, stackScrollbarWidth, scrollbarHeight);
}

void DrawHeaders(Graphics& graphics) {
    FontFamily fontFamily(L"Consolas");
    Font font(&fontFamily, 14, FontStyleRegular, UnitPixel);
    SolidBrush headerBrush(Color(255, 200, 50));
    graphics.DrawString(L"     MEMORY        STACK           REGISTERS", -1, &font, PointF(memoryViewX, memoryViewY - 24), &headerBrush);
}

void Render(Graphics& graphics) {
    DrawChip8Screen(graphics);
    DrawTitle(graphics);
    DrawMemoryPanel(graphics, instance.getMemory());
    DrawStackPanel(graphics, instance.getCpu());
    DrawRegPanel(graphics, instance.getCpu());
    DrawSpecialRegPanel(graphics, instance.getCpu());
    DrawMemScrollbar(graphics);
    DrawStackScrollbar(graphics);
    DrawHeaders(graphics);
}

void EndDoubleBuffering(HDC hdc, PAINTSTRUCT& ps, HDC memDC, HBITMAP oldBitmap, HBITMAP memBitmap) {
    BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);

    EndPaint(WindowFromDC(hdc), &ps);
}

void procKey(WPARAM wParam) {
    switch(wParam) 
        {
            case '1':
                instance.getCpu()->setST(35);
                break;

            case '2':
                instance.getCpu()->setST(35);
                break;

            case '3':
                instance.getCpu()->setST(35);
                break;

            case '4':
                instance.getCpu()->setST(35);
                break;

            case 'Q':
                instance.getCpu()->setST(35);
                break;

            case 'W':
                instance.getCpu()->setST(35);
                break;

            case 'E':
                instance.getCpu()->setST(35);
                break;

            case 'R':
                instance.getCpu()->setST(35);
                break;

            case 'A':
                instance.getCpu()->setST(35);
                break;

            case 'S':
                instance.getCpu()->setST(35);
                break;

            case 'D':
                instance.getCpu()->setST(35);
                break;

            case 'F':
                instance.getCpu()->setST(35);
                break;

            case 'Z':
                instance.getCpu()->setST(35);
                break;
                
            case 'X':
                instance.getCpu()->setST(35);
                break;

            case 'C':
                instance.getCpu()->setST(35);
                break;

            case 'V':
                instance.getCpu()->setST(35);
                break;

            default:
                break;
        }
}

void createButtons(HWND hwnd) {
    HWND startButton = CreateWindow(
        L"BUTTON",
        L"start",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        WINDOW_WIDTH - 290,
        WINDOW_HEIGHT - 148,
        80,
        30,
        hwnd,
        (HMENU)IDC_START,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL
    );

    HWND pauseButton = CreateWindow(
        L"BUTTON",
        L"pause",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_DISABLED,
        WINDOW_WIDTH - 200,
        WINDOW_HEIGHT - 148,
        80,
        30,
        hwnd,
        (HMENU)IDC_PAUSE,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL
    );

    HWND resetButton = CreateWindow(
        L"BUTTON",
        L"reset",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        WINDOW_WIDTH - 290,
        WINDOW_HEIGHT - 113,
        80,
        30,
        hwnd,
        (HMENU)IDC_RESET,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL
    );

    HWND loadButton = CreateWindow(
        L"BUTTON",
        L"load",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        WINDOW_WIDTH - 200,
        WINDOW_HEIGHT - 113,
        80,
        30,
        hwnd,
        (HMENU)IDC_LOAD,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL
    );

    instance.setStartButton(startButton);
    instance.setPauseButton(pauseButton);
    instance.setResetButton(resetButton);
    instance.setLoadButton(loadButton);
}

void excludeButtons(HWND hwnd, HDC hdc) {
    RECT btnRectStart;
        GetWindowRect(instance.getStartButton(), &btnRectStart);
        MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&btnRectStart, 2);
        ExcludeClipRect(hdc, btnRectStart.left, btnRectStart.top, btnRectStart.right, btnRectStart.bottom);

        RECT btnRectPause;
        GetWindowRect(instance.getPauseButton(), &btnRectPause);
        MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&btnRectPause, 2);
        ExcludeClipRect(hdc, btnRectPause.left, btnRectPause.top, btnRectPause.right, btnRectPause.bottom);

        RECT btnRectReset;
        GetWindowRect(instance.getResetButton(), &btnRectReset);
        MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&btnRectReset, 2);
        ExcludeClipRect(hdc, btnRectReset.left, btnRectReset.top, btnRectReset.right, btnRectReset.bottom);

        RECT btnRectLoad;
        GetWindowRect(instance.getLoadButton(), &btnRectLoad);
        MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&btnRectLoad, 2);
        ExcludeClipRect(hdc, btnRectLoad.left, btnRectLoad.top, btnRectLoad.right, btnRectLoad.bottom);
}

void showOpenFileDialog(HWND hwnd) {
    OPENFILENAME f = {sizeof(OPENFILENAMEA)};
    wchar_t buffer[MAX_PATH] = {};

    f.lStructSize = sizeof(f);
    f.hwndOwner = hwnd;
    f.lpstrFile = buffer;
    f.nMaxFile = sizeof(buffer);
    f.lpstrFilter = L"All Files\0*.*\0CHIP-8 ROMs (*.ch8)\0*.ch8\0";
    f.nFilterIndex = 1;
    f.lpstrFileTitle = NULL;
    f.nMaxFileTitle = 0;
    f.lpstrInitialDir = NULL;
    f.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&f) == TRUE)
    {
        instance.getMemory()->loadGame(buffer);
    }
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_MOUSEWHEEL:
    {
        if (GetKeyState(VK_CONTROL) & 0x8000)
            stackScrollWheel(wParam);

        else
            memScrollWheel(wParam);

        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        BeginMemScrollbarDrag(hwnd,lParam);
        BeginStackScrollbarDrag(hwnd, lParam);
        return 0;
    }

    case WM_LBUTTONUP:
        isDraggingMemScrollbar = false;
        isDraggingStackScrollbar = false;
        ReleaseCapture();
        return 0;

    case WM_MOUSEMOVE:
    {
        UpdateMemScrollbarDrag(hwnd, lParam);
        UpdateStackScrollbarDrag(hwnd, lParam);
        return 0;
    }

    case WM_KEYDOWN:
    {
        procKey(wParam);
        break; // To disable "warning: this statement may fall through [-Wimplicit-fallthrough=]"
    }

    case WM_CREATE:
    {
        createButtons(hwnd);
        return 0;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam)){
            case IDC_START:

                instance.getCpu()->resume();

                EnableWindow(instance.getStartButton(), false);
                EnableWindow(instance.getPauseButton(), true);
                break;

            case IDC_PAUSE:
                
                instance.getCpu()->pause();
                
                EnableWindow(instance.getStartButton(), true);
                EnableWindow(instance.getPauseButton(), false);
                break;
                
            case IDC_RESET:
                
                instance.getCpu()->reset();
                instance.clear();

                break;

            case IDC_LOAD:
                showOpenFileDialog(hwnd);
                break;
        }

        break;
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

        excludeButtons(hwnd, hdc);

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

void Display::setCPUPointer(CPU* cpu) {
    this->cpu = cpu;
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