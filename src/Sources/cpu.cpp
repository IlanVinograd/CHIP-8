#include "../Includes/cpu.hpp"

std::atomic<bool> running{true};

static std::atomic<bool> soundPlaying = false;

void CPU::setReg(u8 reg, u8 bitmap) {
    if(reg >= 16) throw std::out_of_range("Out Of Range");
    V[reg] = bitmap;
}

u8 CPU::getReg(u8 reg) const {
    if(reg >= 16) throw std::out_of_range("Out Of Range");
    return V[reg];
}

void CPU::push(u8 byte) {
    if (SP >= 64) throw std::overflow_error("Stack overflow");
    stack[SP++] = byte;
}

void CPU::pop() {
    if (SP == 0) throw std::underflow_error("Stack underflow");
    stack[--SP] = 0;
}

u8 CPU::peek() {
    if (SP == 0) throw std::underflow_error("Stack underflow");
    return stack[SP - 1];
}

u8 CPU::peek(size_t index) const {
    if (index >= SP) throw std::out_of_range("Stack index out of bounds");
    return stack[index];
}

void CPU::beep(CPU* cpu, const wchar_t* path) {
    if (cpu->getST() > 0 && !soundPlaying) {
        soundPlaying = true;
        PlaySound(path, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
    } else if (cpu->getST() == 0 && soundPlaying) {
        PlaySound(NULL, NULL, 0);
        soundPlaying = false;
    }
}

void CPU::chip8TimerLoop(CPU* cpu, HWND hwnd) {
    steady_clock_t::time_point lastTick = steady_clock_t::now();

    wchar_t path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);

    wchar_t* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash) {
        *(lastSlash + 1) = L'\0';
        wcscat_s(path, L"beep2.wav");
    }

    while (running) {
        auto now = steady_clock_t::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTick);

        if (elapsed.count() >= 17) {
            lastTick = now;
            cpu->decDT();
            cpu->decST();

            cpu->beep(cpu, path);

            InvalidateRect(hwnd, NULL, TRUE);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}