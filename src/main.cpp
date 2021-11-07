#include <filesystem>
#include <iostream>

#include "cpu.h"
#include "logger.h"

int main()
{
    const auto roms_path = std::filesystem::current_path() / "roms";
    const auto space_invaders_path = roms_path / "invaders";

    CPU cpu;
    try
    {
        cpu.AddMemory<ROM>(space_invaders_path / "invaders.h");
        cpu.AddMemory<ROM>(space_invaders_path / "invaders.g");
        cpu.AddMemory<ROM>(space_invaders_path / "invaders.f");
        cpu.AddMemory<ROM>(space_invaders_path / "invaders.e");
        cpu.AddMemory<RAM>(0x400);
        cpu.AddMemory<VRAM>(224, 256, cpu); // VRAM
    }
    catch (const std::runtime_error &exception)
    {
        std::cout << exception.what() << std::endl;

        return EXIT_FAILURE;
    }

    return cpu.Run() ? EXIT_SUCCESS : EXIT_FAILURE;
}