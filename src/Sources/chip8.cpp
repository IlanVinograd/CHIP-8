#include "../Includes/chip8.hpp"

bool Chip8::initChip(HINSTANCE hInstance, int nCmdShow, const u8 font[FONT_SIZE]) {
    Display::getInstance().setMemoryPointer(&memory);
    Display::getInstance().setCPUPointer(&cpu);

    memory.loadFont(font);
    memory.loadGame("IBM.ch8");
    
    if (!Display::getInstance().initWindow(hInstance, nCmdShow)) return false;

    // Add all other inits here in the future.

    return true;
}

void Chip8::run() {
    MSG msg = {};

    std::thread timerThread(&CPU::chip8TimerLoop, &cpu, Display::getInstance().getHwnd());

    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                ::running = false;
                break;
            }
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!::running) break;
    }
    
    timerThread.join();
}