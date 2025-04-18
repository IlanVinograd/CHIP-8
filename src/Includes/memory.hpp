#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "../Includes/common.hpp"
#include "../Includes/utility.hpp"

#include <fstream>

#define FONT_OFFSET 0x050

class Memory {
public:
    void loadFont(const u8 font[80]);
    void loadGame(const char* filepath);

    const u8* getRawMemory() const {
        return memory;
    }

    u8 read(u16 address) const {
        if (address < 4096)
            return memory[address];
        return 0;
    }
    
private:
    u8 memory[4096] = { };
};

#endif /*  _MEMORY_H_ */