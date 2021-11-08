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

VRAM::VRAM(unsigned int width, unsigned int height, CPU &cpu) : data_((width / CHAR_BIT) * height), pixels_(width * height * 4), window_thread_running_(true), window_thread_([=, this]()
                                                                                                                                                                              {
#ifdef __linux__
                                                                                                                                                                                  XInitThreads();
#endif // __linux__
                                                                                                                                                                                  sf::RenderWindow window(sf::VideoMode(width, height), "Space Invaders", sf::Style::Close);
                                                                                                                                                                                  //window.setActive(false);

                                                                                                                                                                                  std::atomic<bool> render_thread_running = true;
                                                                                                                                                                                  std::thread render_thread([&, this]()
                                                                                                                                                                                                            {
                                                                                                                                                                                                                //window.setActive(true);
                                                                                                                                                                                                                sf::Texture texture;
                                                                                                                                                                                                                if (!texture.create(width, height))
                                                                                                                                                                                                                {
                                                                                                                                                                                                                    throw std::runtime_error("sf::Texture::create()");
                                                                                                                                                                                                                }

                                                                                                                                                                                                                sf::Sprite sprite(texture);

                                                                                                                                                                                                                while (render_thread_running)
                                                                                                                                                                                                                {
                                                                                                                                                                                                                    std::vector<uint8_t> data_copy;
                                                                                                                                                                                                                    {
                                                                                                                                                                                                                        std::scoped_lock lock(data_mutex_);
                                                                                                                                                                                                                        data_copy = data_;
                                                                                                                                                                                                                    }
                                                                                                                                                                                                                    //std::memset(data_copy.data(), 255, data_copy.size());

                                                                                                                                                                                                                    std::size_t pos = 0;
                                                                                                                                                                                                                    for (std::size_t x = 0; x < width; ++x)
                                                                                                                                                                                                                    {
                                                                                                                                                                                                                        for (std::size_t y = 0; y < height;)
                                                                                                                                                                                                                        {
                                                                                                                                                                                                                            uint8_t pixels_octet = data_copy[pos++];
                                                                                                                                                                                                                            for (uint8_t b = 0; b < 8; ++b)
                                                                                                                                                                                                                            {
                                                                                                                                                                                                                                std::memset(&pixels_[(height - 1 - y) * width * 4 + x * 4], pixels_octet & (1 << b) ? 255 : 0, 4);

                                                                                                                                                                                                                                ++y;
                                                                                                                                                                                                                            }
                                                                                                                                                                                                                        }
                                                                                                                                                                                                                    }
                                                                                                                                                                                                                    texture.update(pixels_.data());
                                                                                                                                                                                                                    cpu_.Interrupt(1);
                                                                                                                                                                                                                    window.clear();
                                                                                                                                                                                                                    window.draw(sprite);
                                                                                                                                                                                                                    window.display();
                                                                                                                                                                                                                    cpu_.Interrupt(2);

                                                                                                                                                                                                                    std::this_thread::sleep_for(1ms);
                                                                                                                                                                                                                }
                                                                                                                                                                                                            });

                                                                                                                                                                                  const auto close_window = [&, this]()
                                                                                                                                                                                  {
                                                                                                                                                                                      render_thread_running = false;
                                                                                                                                                                                      render_thread.join();
                                                                                                                                                                                      window.close();
                                                                                                                                                                                      cpu_.Stop();
                                                                                                                                                                                  };

                                                                                                                                                                                  while (window.isOpen())
                                                                                                                                                                                  {
                                                                                                                                                                                      sf::Event event;
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
                                                                cpu_(cpu)

{
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