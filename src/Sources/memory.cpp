#include "../Includes/memory.hpp"

void Memory::loadFont(const u8 font[FONT_SIZE]) {
    for(int i = 0; i < FONT_SIZE; i++) {
        memory[i + FONT_OFFSET] = font[i];
    }
}