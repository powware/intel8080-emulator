#include <filesystem>
#include <iostream>

#include <SFML/Window.hpp>

#include "cpu.h"
#include "logger.h"

int main()
{
    const auto roms_path = std::filesystem::current_path() / "roms";
    const auto space_invaders_path = roms_path / "invaders";

    sf::Window window(sf::VideoMode(800, 600), "Space Invaders - Intel8080 Emulator", sf::Style::Close);

    CPU cpu;
    try
    {
        cpu.AddROM(space_invaders_path / "invaders.h");
        cpu.AddROM(space_invaders_path / "invaders.g");
        cpu.AddROM(space_invaders_path / "invaders.f");
        cpu.AddROM(space_invaders_path / "invaders.e");
        cpu.AddRAM(0x400);
        cpu.AddRAM(0x1600); // VRAM
        cpu.Start();
    }
    catch (const std::runtime_error &exception)
    {
        std::cout << exception.what() << std::endl;

        cpu.Stop();
        window.close();
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
            {
                cpu.Stop();
                window.close();
            }
            break;
            default:
            {
            }
            break;
            }
        }
    }

    return 0;
}