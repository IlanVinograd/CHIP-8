#ifndef _CHIP8_H_
#define _CHIP8_H_

#include "../Includes/common.hpp"
#include "../Includes/display.hpp"

class Chip8 {
public:
    bool initChip(HINSTANCE hInstance, int nCmdShow);
    void run();

private:
    Display display;
};

#endif /* _CHIP8_H_ */