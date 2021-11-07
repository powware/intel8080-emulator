#ifndef VRAM_H
#define VRAM_H

#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>

#include "memory_interface.h"

class CPU;

class VRAM final : public MemoryInterface
{
public:
    VRAM(unsigned int width, unsigned int height, CPU &cpu);

    VRAM(const VRAM &) = delete;

    VRAM(VRAM &&) = delete;

    virtual ~VRAM();

    auto operator=(const VRAM &) = delete;

    auto operator=(VRAM &&) = delete;

    std::size_t size() const noexcept override;

    uint8_t Read(std::size_t index) const override;

    void Write(std::size_t index, uint8_t data) noexcept override;

private:
    std::vector<uint8_t> pixels_;

    sf::RenderWindow window_;

    std::atomic<bool> event_handler_thread_running_;
    std::thread event_handler_thread_;

    std::atomic<bool> render_thread_running_;
    std::thread render_thread_;

    CPU &cpu_;
};

#endif /* VRAM_H */
