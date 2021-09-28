#include <filesystem>

#include "cpu.h"

int main()
{

    auto rom_path = std::filesystem::current_path() / "roms";
    auto space_invaders_path = rom_path / "invaders";

    CPU cpu;
    cpu.AddROM(space_invaders_path / "invaders.e");
    cpu.AddROM(space_invaders_path / "invaders.f");
    cpu.AddROM(space_invaders_path / "invaders.g");
    cpu.AddROM(space_invaders_path / "invaders.h");
    cpu.AddRAM(0x400);
    cpu.AddRAM(0x1600);

    cpu.StartExecution();

    return 0;
}