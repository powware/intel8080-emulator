#include <filesystem>
#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <string>
#include <sstream>

#include "cpu.h"
#include "rom.h"
#include "memory_map.h"

void HexDump(const MemoryMap &memory)
{
    const char *numbers = "0123456789ABCDEF";
    for (uint16_t i = 0; i < static_cast<uint16_t>(memory.GetSize()); i += 0x10)
    {
        std::cout << numbers[(i & 0x0F000) >> (3 * 4)];
        std::cout << numbers[(i & 0x00F00) >> (2 * 4)];
        std::cout << numbers[(i & 0x000F0) >> (1 * 4)];
        std::cout << numbers[(i & 0x0000F)];

        for (int j = 0; j < 0xF; j++)
        {
            uint8_t value;
            if (!memory.Read(i, value))
            {
                return;
            }

            std::cout << " ";
            std::cout << numbers[(value & 0xF0) >> (1 * 4)];
            std::cout << numbers[(value & 0x0F)];
        }

        std::cout << std::endl;
    }
}

int main()
{
    // CPU cpu();

    auto rom_path = std::filesystem::current_path() / "roms";
    auto space_invaders_path = rom_path / "invaders";

    MemoryMap memory_map(Rom(space_invaders_path / "invaders.e", space_invaders_path / "invaders.f", space_invaders_path / "invaders.g", space_invaders_path / "invaders.h"));
    HexDump(memory_map);
    return 0;
}