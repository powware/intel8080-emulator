#include "vram.h"

#include <chrono>
#include <cstring>

#ifdef __linux__
#include <X11/Xlib.h>
#endif // __linux__

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

#include "cpu.h"

using namespace std::chrono_literals;

VRAM::VRAM(CPU &cpu) : window_thread_running_(true), cpu_(cpu)
{
    window_thread_ = std::thread([=, this]()
                                 {
#ifdef __linux__
                                     XInitThreads();
#endif // __linux__
                                     sf::RenderWindow window(sf::VideoMode(kWidth, kHeight), "Space Invaders", sf::Style::Close);
                                     window.setActive(false);

                                     std::atomic<bool> render_thread_running = true;
                                     std::thread render_thread([&, this]()
                                                               {
                                                                   window.setActive(true);
                                                                   sf::Texture texture;
                                                                   if (!texture.create(kWidth, kHeight))
                                                                   {
                                                                       throw std::runtime_error("sf::Texture::create()");
                                                                   }

                                                                   sf::Sprite sprite(texture);
                                                                   while (render_thread_running)
                                                                   {
                                                                       std::array<uint8_t, kWidth *(kHeight / CHAR_BIT)> data_copy;
                                                                       {
                                                                           std::scoped_lock lock(data_mutex_);
                                                                           data_copy = data_;
                                                                       }

                                                                       for (std::size_t y = 0; y < kHeight / CHAR_BIT; ++y)
                                                                       {
                                                                           for (std::size_t x = 0; x < kWidth; ++x)
                                                                           {
                                                                               for (uint8_t b = 0; b < CHAR_BIT; ++b)
                                                                               {
                                                                                   std::memset(&pixels_[(kHeight - 1 - (y * CHAR_BIT + b)) * kWidth + x], data_copy[x * (kHeight / CHAR_BIT) + y] & (1 << b) ? 255 : 0, sizeof(uint32_t));
                                                                               }
                                                                           }
                                                                       }

                                                                       texture.update(reinterpret_cast<uint8_t *>(pixels_.data()));
                                                                       window.clear();
                                                                       window.draw(sprite);
                                                                       window.display();

                                                                       constexpr auto kSleepInterval = (1s / 60.0) / 2.0; // 60hz devided into 2 sleep intervals
                                                                       cpu_.Interrupt(1);
                                                                       std::this_thread::sleep_for(kSleepInterval);
                                                                       cpu_.Interrupt(2);
                                                                       std::this_thread::sleep_for(kSleepInterval);
                                                                   }

                                                                   window.setActive(false);
                                                               });

                                     const auto close_window = [&, this]()
                                     {
                                         render_thread_running = false;
                                         render_thread.join();
                                         window.close();
                                         cpu_.Stop();
                                     };

                                     sf::Event event;
                                     while (window.isOpen())
                                     {
                                         while (window.pollEvent(event))
                                         {
                                             switch (event.type)
                                             {
                                             case sf::Event::Closed:
                                             {
                                                 close_window();
                                             }
                                             break;
                                             default:
                                             {
                                             }
                                             break;
                                             }
                                         }

                                         if (!window_thread_running_)
                                         {
                                             close_window();
                                         }
                                     }
                                 }),
    std::memset(data_.data(), 0, data_.size());
}

VRAM::~VRAM()
{
    window_thread_running_ = false;
    window_thread_.join();
}

std::size_t VRAM::size() const noexcept
{
    return data_.size();
}

uint8_t VRAM::Read(std::size_t index) const
{
    std::scoped_lock lock(data_mutex_);
    return data_[index];
}

void VRAM::Write(std::size_t index, uint8_t data) noexcept
{
    std::scoped_lock lock(data_mutex_);
    data_[index] = data;
}