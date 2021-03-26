//
// Created by Maxwell on 2020-11-23.
//

#ifndef VALKYRIE_GUI_WINDOW_HPP
#define VALKYRIE_GUI_WINDOW_HPP

#include <valkyrie/gui/monitor.hpp>
#include <valkyrie/status/status_code.hpp>

namespace valkyrie::gui{
  class window {
    class Impl;
    Impl* pImpl;
  public:
    explicit window(Extent2D<> windowSize);
    explicit window(Rect2D windowRectangle);
    window(Extent2D<> windowSize, monitor monitor);
    window(Rect2D windowRectangle, monitor monitor);

    window(const window &);
    window(window &&) noexcept;
    window & operator=(const window &);
    window & operator=(window &&) noexcept;
    ~window();


    monitor         monitor() const noexcept;
    Rect2D          getArea() const noexcept;
    Position2D<i32> getPosition() const noexcept;
    Extent2D<>      getExtents() const noexcept;


    status setFullscreen(bool enableFullscreen = true) noexcept;
    status close() noexcept;
  };
}


#endif//VALKYRIE_GUI_WINDOW_HPP
