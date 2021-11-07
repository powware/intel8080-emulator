#include "vram.h"

#include <chrono>
#include <cstring>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

#include "cpu.h"

using namespace std::chrono_literals;

VRAM::VRAM(unsigned int width, unsigned int height, CPU &cpu) : pixels_(width * height * 4), window_(sf::VideoMode(width, height), "Space Invaders", sf::Style::Close), event_handler_thread_running_(true),
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
                                                                render_thread_running_(true), cpu_(cpu)
{
    window_.setActive(false);

    std::memset(pixels_.data(), 0, pixels_.size());

    render_thread_ = std::thread([=, this]()
                                 {
                                     window_.setActive(true);
                                     sf::Texture texture;
                                     if (!texture.create(width, height))
                                     {
                                         throw std::runtime_error("sf::Texture::create()");
                                     }
                                     sf::Sprite sprite(texture);
                                     while (render_thread_running_)
                                     {
                                         texture.update(pixels_.data());

                                         cpu_.Interrupt(1);
                                         window_.clear();
                                         window_.draw(sprite);
                                         window_.display();
                                         cpu_.Interrupt(2);

                                         std::this_thread::sleep_for(1ms);
                                     }
                                 });
}

VRAM::~VRAM()
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

std::size_t VRAM::size() const noexcept
{
    return pixels_.size() / 4;
}

uint8_t VRAM::Read(std::size_t index) const
{
    return pixels_[index * 4];
}

void VRAM::Write(std::size_t index, uint8_t data) noexcept
{
    data = 255;
    std::memset(&pixels_[index * 4], data, 4);
}