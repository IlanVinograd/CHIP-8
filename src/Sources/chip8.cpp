#include "../Includes/chip8.hpp"

bool Chip8::initChip(HINSTANCE hInstance, int nCmdShow, const u8 font[FONT_SIZE]) {
    instance.setMemoryPointer(&memory);
    instance.setCPUPointer(&cpu);

    memory.loadFont(font);
    memory.loadGame(L"../Games/IBM.ch8");
    
    if (!instance.initWindow(hInstance, nCmdShow)) return false;

    return true;
}

void Chip8::run() {
    MSG msg = {};

    std::thread timerThread(&CPU::chip8TimerLoop, &cpu, instance.getHwnd());

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