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

void CPU::push(u16 addr) {
    if (SP >= 64) throw std::overflow_error("Stack overflow");
    stack[SP++] = addr;
}

void CPU::pop() {
    if (SP == 0) throw std::underflow_error("Stack underflow");
    stack[--SP] = 0;
}

u16 CPU::peek() {
    if (SP == 0) throw std::underflow_error("Stack underflow");
    return stack[SP - 1];
}

u16 CPU::peek(size_t index) const {
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
    steady_clock_t::time_point lastTickTimer = steady_clock_t::now();
    steady_clock_t::time_point lastTickExe = steady_clock_t::now();

    wchar_t path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);

    wchar_t* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash) {
        *(lastSlash + 1) = L'\0';
        wcscat_s(path, L"beep.wav");
    }

    while (running) {
        if (!IsWindowEnabled(hwnd)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        auto now = steady_clock_t::now();

        auto elapsedTimer = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTickTimer);
        auto elapsedExe = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTickExe);
        

        if (elapsedTimer.count() >= SCREEN_MS) {
            lastTickTimer = now;
            cpu->decDT();
            cpu->decST();

            cpu->beep(cpu, path);

            InvalidateRect(hwnd, NULL, TRUE);
        }

        if(elapsedExe.count() >= CPU_MS) {
            lastTickExe = now;

            if (!cpu->isPaused()) cpu->FetchDecodeExecute();

            InvalidateRect(hwnd, NULL, TRUE);
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1)); // <---- DONT FORGOT TO CHANGE BACK.
    }
}

u16 CPU::fetch() {
    u8 high = instance.getMemory()->read(PC);
    u8 low = instance.getMemory()->read(PC+1);

    u16 opcode = (high << 8) | low;
    PC += 2;

    return opcode;
}

void CPU::decodeAndExecute(u16 &opcode) {
    u8 op  = (opcode & 0xF000) >> 12;
    u8 x   = (opcode & 0x0F00) >> 8;
    u8 y   = (opcode & 0x00F0) >> 4;
    u8 n   = (opcode & 0x000F);
    u8 nn  = (opcode & 0x00FF);
    u16 nnn = (opcode & 0x0FFF);

    switch (op) {

        case 0x0:
            if (x == 0x0) {
                switch (nn) {
                    case 0xE0: 
                        instance.clear(); // Clear Screan.
                        break;

                    case 0xEE: { // RET Return from a subroutine (00EE instruction).
                        u16 returnAddress = instance.getCpu()->peek();
                        instance.getCpu()->pop();
                        instance.getCpu()->PC = returnAddress;
                        break;

                    }

                    default:
                        throw "ERROR Unrecognized Opcode On: 0x00EE / 0x00E0 ";
                        break;
                }
            } else {
                throw "ERROR Unrecognized Opcode On: 0x0X00 <- Should be 0";
            }
            break;

        case 0x1:
            instance.getCpu()->PC = nnn; // Jumping to nnn (PC = nnn).
            break;

        case 0x2: // Call a subroutine (2nnn instruction).
            instance.getCpu()->push(instance.getCpu()->PC);
            instance.getCpu()->PC = nnn;
            break;

        case 0x3: // Skip next instruction if Vx == nn (3xnn instruction).
            if(instance.getCpu()->getReg(x) == nn) instance.getCpu()->PC += 2; 
            break;

        case 0x4: // Skip next instruction if Vx != nn (4xnn instruction).
            if(instance.getCpu()->getReg(x) != nn) instance.getCpu()->PC += 2; 
            break;

        case 0x5: // Skip next instruction if Vx = Vy (5xy0 instruction).
            if(instance.getCpu()->getReg(x) == instance.getCpu()->getReg(y)) instance.getCpu()->PC += 2;
            break;

        case 0x6:
            instance.getCpu()->setReg(x, nn); // Set Vx = nn (6xnn instruction).
            break;

        case 0x7: {
            u8 Vx = instance.getCpu()->getReg(x);
            instance.getCpu()->setReg(x, Vx + nn); // Adds the value nn to the value of register Vx (7xnn instruction). 
            break;
        }

        case 0x8:
            switch (n) {
                case 0x0: {
                    instance.getCpu()->setReg(x, instance.getCpu()->getReg(y));
                    break;
                }

                case 0x1: {
                    instance.getCpu()->setReg(x, instance.getCpu()->getReg(x) | instance.getCpu()->getReg(y));
                    break;
                }

                case 0x2: {
                    instance.getCpu()->setReg(x, instance.getCpu()->getReg(x) & instance.getCpu()->getReg(y));
                    break;
                }

                case 0x3: {
                    instance.getCpu()->setReg(x, instance.getCpu()->getReg(x) ^ instance.getCpu()->getReg(y));
                    break;
                }

                case 0x4: {
                    u16 sum = instance.getCpu()->getReg(x) + instance.getCpu()->getReg(y);
                    instance.getCpu()->setReg(0xF, sum > 255 ? 1 : 0);
                    instance.getCpu()->setReg(x, static_cast<u8>(sum));
                    break;
                }

                case 0x5: {
                    u8 Vx = instance.getCpu()->getReg(x);
                    u8 Vy = instance.getCpu()->getReg(y);
                    instance.getCpu()->setReg(0xF, Vx >= Vy ? 1 : 0);
                    instance.getCpu()->setReg(x, Vx - Vy);
                    break;
                }

                case 0x6: {
                    if (useLegacyShiftMode) { // Not Implemented yet.

                    } else {
                        u8 Vx = instance.getCpu()->getReg(x);
                        instance.getCpu()->setReg(0xF, Vx & 0x1);
                        instance.getCpu()->setReg(x, Vx >> 1);
                    }
                    break;
                }

                case 0x7: {
                    u8 Vx = instance.getCpu()->getReg(x);
                    u8 Vy = instance.getCpu()->getReg(y);
                    instance.getCpu()->setReg(0xF, Vy >= Vx ? 1 : 0);
                    instance.getCpu()->setReg(x, Vy - Vx);
                    break;
                }

                case 0xE: {
                    if (useLegacyShiftMode) { // Not Implemented yet.

                    } else {
                        u8 Vx = instance.getCpu()->getReg(x);
                        instance.getCpu()->setReg(0xF, (Vx >> 7) & 0x1);
                        instance.getCpu()->setReg(x, Vx << 1);
                    }
                    break;
                }

                default: 
                    throw "ERROR Unrecognized Opcode On: 8XYN Instructions.";
                    break;
            }
            break;

        case 0x9: {
            if (n == 0) {
                if (instance.getCpu()->getReg(x) != instance.getCpu()->getReg(y))
                    instance.getCpu()->PC += 2;
            } else {
                throw "ERROR Unrecognized Opcode On: 9XYN (n != 0)";
            }
            break;
        }

        case 0xA:
            instance.getCpu()->I = nnn; // The value of register I is set to nnn (Annn instruction).
            break;

        case 0xB:
            if (useLegacyShiftMode) { // Not Not Implemented yet.

            } else {
                instance.getCpu()->PC = nnn + instance.getCpu()->getReg(0x0);
            }
            break;

        case 0xC: {
            u8 randomNumber = std::rand() % 256;
            instance.getCpu()->setReg(x, randomNumber & nn);
            break;
        }
        case 0xD: { // Draw on screen.
            u8 vx = instance.getCpu()->getReg(x);
            u8 vy = instance.getCpu()->getReg(y);
        
            instance.getCpu()->setReg(0xF, 0);
        
            for (int row = 0; row < n; row++) {
                if ((vy + row) >= HEIGHT) break;
        
                u8 spriteByte = instance.getMemory()->read(I + row);
        
                for (int col = 0; col < 8; col++) {
                    if ((vx + col) >= WIDTH) break;
        
                    u8 spritePixel = (spriteByte >> (7 - col)) & 0x1;
        
                    if (spritePixel) {
                        bool erased = instance.togglePixel(vx + col, vy + row);
                        if (erased)
                            instance.getCpu()->setReg(0xF, 1);
                    }
                }
            }
            break;
        }

        case 0xE: 
            if(nn == 0x9E) { 
                // if key with the value in Vx is pressed.
                if(instance.isKeyPressed(instance.getCpu()->getReg(x))) 
                    instance.getCpu()->PC += 2;
            } else if (nn == 0xA1) {
                // if key with the value in Vx is not pressed
                if(!instance.isKeyPressed(instance.getCpu()->getReg(x))) 
                    instance.getCpu()->PC += 2;
            } else {
                throw "ERROR Unrecognized Opcode On: Ex9E / ExA1";
            }
            break;

        case 0xF:
            switch (nn) {
                case 0x07:
                    instance.getCpu()->setReg(x, instance.getCpu()->getDT());
                    break;

                case 0x0A: {
                    for (int i = 0; i < 16; ++i) {
                        if (instance.isKeyPressed(i)) {
                            instance.getCpu()->setReg(x, i);
                            return;
                        }
                    }
                
                    instance.getCpu()->PC -= 2;
                    return;
                }

                case 0x15:
                    instance.getCpu()->setDT(instance.getCpu()->getReg(x));
                    break;

                case 0x18:
                    instance.getCpu()->setST(instance.getCpu()->getReg(x));
                    break;

                case 0x1E: {
                    u16 Vx = instance.getCpu()->getReg(x);
                    u16 I = instance.getCpu()->I;
                    u16 result = I + Vx;
                
                    instance.getCpu()->I = result;
                
                    if (useAmigaOverflowMode)
                        instance.getCpu()->setReg(0xF, result > 0x0FFF ? 1 : 0);
                
                    break;
                }
                
                case 0x29: {
                    u8 digit = instance.getCpu()->getReg(x) & 0x0F;
                    instance.getCpu()->I = digit * 5;
                    break;
                }                
                    
                case 0x33: {
                    u8 value = instance.getCpu()->getReg(x);
                    u16 I = instance.getCpu()->I;
                
                    instance.getMemory()->write(I + 2, value % 10);
                    value /= 10;
                    instance.getMemory()->write(I + 1, value % 10);
                    value /= 10;
                    instance.getMemory()->write(I, value % 10);
                
                    break;
                }

                case 0x55: {
                    u16 I = instance.getCpu()->I;
                    for (int i = 0; i <= x; ++i)
                        instance.getMemory()->write(I + i, instance.getCpu()->getReg(i));
                
                    if (useOriginalStoreLoadMode)
                        instance.getCpu()->I += x + 1;
                
                    break;
                }
                
                case 0x65: {
                    u16 I = instance.getCpu()->I;
                    for (int i = 0; i <= x; ++i)
                        instance.getCpu()->setReg(i, instance.getMemory()->read(I + i));
                
                    if (useOriginalStoreLoadMode)
                        instance.getCpu()->I += x + 1;
                
                    break;
                }
            }

            break;

        default:
            throw "ERROR DEFAULT EXCEPTION";
            return;
    }
}

void CPU::FetchDecodeExecute() {
    u16 opcode = fetch();
    try {
        decodeAndExecute(opcode);
    } catch (const std::exception& e) {
        MessageBoxA(NULL, e.what(), "Standard Exception", MB_ICONERROR | MB_OK);
    } catch (const char* msg) {
        MessageBoxA(NULL, msg, "Custom Error", MB_ICONERROR | MB_OK);
    } catch (...) {
        MessageBoxA(NULL, "Unknown exception occurred", "Unknown Error", MB_ICONERROR | MB_OK);
    }
}

void CPU::reset() {
    I = 0;
    PC = 0x200;

    SP = 0;
    std::fill(std::begin(stack), std::end(stack), 0);

    std::fill(std::begin(V), std::end(V), 0);

    DT = 0;
    ST = 0;

    lastTick = steady_clock_t::now();
}