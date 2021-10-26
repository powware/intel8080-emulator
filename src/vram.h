#ifndef VRAM_H
#define VRAM_H

#include <thread>
#include <atomic>
#include <chrono>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

#include "memory_interface.h"

using namespace std::chrono_literals;

class VRAM final : public MemoryInterface
{
public:
    VRAM(unsigned int width, unsigned int height) : pixels_(width * height * 4), window_(sf::VideoMode(width, height), "Space Invaders", sf::Style::Close), event_handler_thread_running_(true),
                                                    event_handler_thread_([this]()
                                                                          {
                                                                              while (window_.isOpen() && event_handler_thread_running_)
                                                                              {
                                                                                  sf::Event event;
                                                                                  while (window_.pollEvent(event))
                                                                                  {
                                                                                      switch (event.type)
                                                                                      {
                                                                                      case sf::Event::Closed:
                                                                                      {
                                                                                          window_.close();
                                                                                      }
                                                                                      break;
                                                                                      default:
                                                                                      {
                                                                                      }
                                                                                      break;
                                                                                      }
                                                                                  }
                                                                              }
                                                                          }),
                                                    render_thread_running_(true), render_thread_([=, this]()
                                                                                                 {
                                                                                                     sf::Texture texture;
                                                                                                     if (!texture.create(width, height))
                                                                                                     {
                                                                                                         throw std::runtime_error("texture_.create");
                                                                                                     }
                                                                                                     sf::Sprite sprite(texture);
                                                                                                     while (render_thread_running_)
                                                                                                     {

                                                                                                         texture.update(pixels_.data());

                                                                                                         window_.clear();
                                                                                                         window_.draw(sprite);
                                                                                                         window_.display();

                                                                                                         std::this_thread::sleep_for(1ms);
                                                                                                     }
                                                                                                 })
    {
    }

    VRAM(const VRAM &) = delete;

    VRAM(VRAM &&) = delete;

    virtual ~VRAM()
    {
        render_thread_running_ = false;
        render_thread_.join();

        event_handler_thread_running_ = false;
        event_handler_thread_.join();

        if (window_.isOpen())
        {
            window_.close();
        }
    }

    auto operator=(const VRAM &) = delete;

    auto operator=(VRAM &&) = delete;

    std::size_t size() const noexcept override
    {
        return 0;
    }

    uint8_t Read(std::size_t index) const noexcept override
    {
        unused(index);

        return 0;
    }

    void Write(std::size_t index, uint8_t data) noexcept override
    {
        unused(index, data);
    }

private:
    std::vector<uint8_t> pixels_;
    sf::RenderWindow window_;
    std::atomic<bool> event_handler_thread_running_;
    std::thread event_handler_thread_;
    std::atomic<bool> render_thread_running_;
    std::thread render_thread_;
};

#endif /* VRAM_H */
