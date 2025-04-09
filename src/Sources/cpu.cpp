#include "../Includes/cpu.hpp"

void CPU::setReg(u8 reg, u8 bitmap) {
    if(reg > 16) throw std::out_of_range("Out Of Range");
    V[reg] = bitmap;
}

u8 CPU::getReg(u8 reg) const {
    return V[reg];
}

void CPU::push(u8 byte) {
    SP++;
    stack[SP] =  byte;
}

void CPU::pop() {
    stack[SP] = 0;
    SP--;
}

u8 CPU::peek() {
    return stack[SP];
}