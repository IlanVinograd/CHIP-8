#ifndef _CPU_H_
#define _CPU_H_

#include "../Includes/common.hpp"

#include <chrono>
#include <atomic>

#define SCREEN_MS 17
#define CPU_MS 3

using steady_clock_t = std::chrono::steady_clock;

extern std::atomic<bool> running;

class CPU {
public:
    // Methods for registers.
    void setReg(u8 reg, u8 bitmap);
    u8 getReg(u8 reg) const;

    // Methods for stack.
    void push(u8 byte);
    void pop();
    u8 peek();
    u8 peek(size_t index) const;
    
    // CPU Methods
    u16 getI() const { return I; };
    u16 getPC() const { return PC; };

    u8 getSP() const { return SP; };
    u8 getST() const { return ST; };
    u8 getDT() const { return DT; };

    void setDT(u8 byte) { DT = byte; }
    
    void setST(u8 byte) { ST = byte; }

    void beep(CPU* cpu, const wchar_t* path);
    static void chip8TimerLoop(CPU* cpu, HWND hwnd);
    void stop() { running = false; }

private:
    u16 I  = 0x0;    // Is 16 bit but we use only 12 bit | 0000 [ 1111 1111 1111 ] |
    u16 PC = 0x200;

    u8 stack[64] = { };
    u8 SP  = 0x0;

    u8 V[16] = { };
    u8 DT  = 0x0;
    u8 ST  = 0x0;

    steady_clock_t::time_point lastTick = steady_clock_t::now();

    void decST() {if(ST > 0) ST--; };
    void decDT() { if(DT > 0) DT--; };
};

#endif /* _CPU_H_ */