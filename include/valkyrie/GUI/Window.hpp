//
// Created by Maxwell on 2020-11-23.
//

#ifndef VALKYRIE_GUI_WINDOW_HPP
#define VALKYRIE_GUI_WINDOW_HPP

#include <valkyrie/GUI/Monitor.hpp>
#include <valkyrie/Core/Error/Status.hpp>

namespace valkyrie::GUI{
  class Window{
    class Impl;
    Impl* pImpl;
  public:
    Window(Graphics::Extent2D<> windowSize);
    Window(Graphics::Box2D<i32> windowRectangle);
    Window(Graphics::Extent2D<> windowSize, Monitor monitor);
    Window(Graphics::Box2D<i32> windowRectangle, Monitor monitor);

    Window(const Window&);
    Window(Window&&) noexcept;
    Window& operator=(const Window&);
    Window& operator=(Window&&) noexcept;
    ~Window();


    Monitor                   monitor() const noexcept;
    Graphics::Box2D<i32>      getArea() const noexcept;
    Graphics::Position2D<i32> getPosition() const noexcept;
    Graphics::Extent2D<>      getExtents() const noexcept;


    Core::Status setFullscreen(bool enableFullscreen = true) noexcept;
    Core::Status close() noexcept;
  };
}


#endif//VALKYRIE_GUI_WINDOW_HPP
