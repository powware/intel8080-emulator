#ifndef VRAM_H
#define VRAM_H

#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>
#include <array>

#include <SFML/Graphics/RenderWindow.hpp>

#include "memory_interface.h"

class CPU;

class VRAM final : public MemoryInterface
{
public:
    VRAM(CPU &cpu);

    VRAM(const VRAM &) = delete;

    VRAM(VRAM &&) = delete;

    virtual ~VRAM();

    auto operator=(const VRAM &) = delete;

    auto operator=(VRAM &&) = delete;

    std::size_t size() const noexcept override;

    uint8_t Read(std::size_t index) const override;

    void Write(std::size_t index, uint8_t data) noexcept override;

private:
    constexpr static unsigned int kWidth = 224;
    constexpr static unsigned int kHeight = 256;

    mutable std::mutex data_mutex_;
    std::array<uint8_t, kWidth *(kHeight / CHAR_BIT)> data_;
    std::array<uint32_t, kWidth * kHeight> pixels_;

    std::atomic<bool> window_thread_running_;
    std::thread window_thread_;

    CPU &cpu_;
};

#endif /* VRAM_H */
