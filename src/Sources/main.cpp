#include "../Includes/common.hpp"
#include "../Includes/chip8.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    Chip8 chip8;
    
    if(!chip8.initChip(hInstance, nCmdShow)) {
        return -1;
    }

    chip8.run();

    return 0;
}