//
// Created by Maxwell on 2020-11-23.
//

#ifndef VALKYRIE_GUI_WINDOW_HPP
#define VALKYRIE_GUI_WINDOW_HPP

#include <valkyrie/gui/monitor.hpp>
#include <valkyrie/status/status_code.hpp>

namespace valkyrie::GUI{
  class window {
    class Impl;
    Impl* pImpl;
  public:
    window(graphics::Extent2D<> windowSize);
    window(graphics::Box2D<i32> windowRectangle);
    window(graphics::Extent2D<> windowSize, monitor monitor);
    window(graphics::Box2D<i32> windowRectangle, monitor monitor);

    window(const window &);
    window(window &&) noexcept;
    window & operator=(const window &);
    window & operator=(window &&) noexcept;
    ~window();


    monitor monitor() const noexcept;
    graphics::Box2D<i32>      getArea() const noexcept;
    graphics::Position2D<i32> getPosition() const noexcept;
    graphics::Extent2D<>      getExtents() const noexcept;


    status setFullscreen(bool enableFullscreen = true) noexcept;
    status close() noexcept;
  };
}


#endif//VALKYRIE_GUI_WINDOW_HPP
