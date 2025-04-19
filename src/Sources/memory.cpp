#include "../Includes/memory.hpp"

void Memory::loadFont(const u8 font[FONT_SIZE]) {
    for(int i = 0; i < FONT_SIZE; i++) {
        memory[i + FONT_OFFSET] = font[i];
    }
}

void Memory::loadGame(const wchar_t* filepath) {
    HANDLE hFile = CreateFileW(
        filepath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Can't open ROM file (WinAPI)");
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE || fileSize > (4096 - 0x200)) {
        CloseHandle(hFile);
        throw std::runtime_error("ROM too large or invalid size");
    }

    DWORD bytesRead = 0;
    BOOL success = ReadFile(hFile, &memory[0x200], fileSize, &bytesRead, NULL);
    CloseHandle(hFile);

    if (!success || bytesRead != fileSize) {
        throw std::runtime_error("Failed to read entire ROM file");
    }
}