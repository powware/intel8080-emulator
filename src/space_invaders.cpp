#include <filesystem>
#include <iostream>

#include "cpu.h"
#include "ram.h"
#include "rom.h"
#include "vram.h"

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
        cpu.AddMemory<VRAM>(cpu);
        cpu.Run();
    }
    catch (const std::exception &exception)
    {
        std::cout << exception.what() << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#ifdef _WIN32
#include <Windows.h>

#include "utilities.h"

int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    unused(hInst, hInstPrev, cmdline, cmdshow);

    return main();
}
#endif