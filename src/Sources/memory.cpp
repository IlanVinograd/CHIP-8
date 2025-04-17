#include "../Includes/memory.hpp"

void Memory::loadFont(const u8 font[FONT_SIZE]) {
    for(int i = 0; i < FONT_SIZE; i++) {
        memory[i + FONT_OFFSET] = font[i];
    }
}

void Memory::loadGame(const char* filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Failed to open ROM file");
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    u16 pc = 0x200;
    if (size > (4096 - pc)) {
        throw std::runtime_error("ROM too large to fit in memory");
    }

    file.read(reinterpret_cast<char*>(&memory[pc]), size);
    file.close();
}