#ifndef _CHIP8_H_
#define _CHIP8_H_

#include "../Includes/common.hpp"
#include "../Includes/display.hpp"
#include "../Includes/memory.hpp"
#include "../Includes/utility.hpp"

class Chip8 {
public:
    bool initChip(HINSTANCE hInstance, int nCmdShow, const u8 font[80]);
    void run();

private:
    Memory memory;
};

#endif /* _CHIP8_H_ */