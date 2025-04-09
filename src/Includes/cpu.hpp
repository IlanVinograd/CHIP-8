#ifndef _CPU_H_
#define _CPU_H_

#include "../Includes/common.hpp"

class CPU {
public:
    // Methods for registers.
    void setReg(u8 reg, u8 bitmap);
    u8 getReg(u8 reg) const;

    // Methods for stack.
    void push(u8 byte);
    void pop();
    u8 peek();
    
    // CPU Methods
    //

private:
    u16 I  = 0x0;    // Is 16 bit but we use only 12 bit | 0000 [ 1111 1111 1111 ] |
    u16 PC = 0x200;

    u8 stack[64] = { };
    u8 SP  = 0;

    u8 V[16] = { };
    u8 DT  = 0x0;
    u8 ST  = 0x0;
};

#endif /* _CPU_H_ */