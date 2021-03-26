//
// Created by Maxwell on 2020-11-23.
//

#ifndef VALKYRIE_GUI_MONITOR_HPP
#define VALKYRIE_GUI_MONITOR_HPP

#include <valkyrie/graphics/pixel.hpp>
#include <valkyrie/utility/shapes.hpp>
#include <valkyrie/status/status_code.hpp>

namespace valkyrie::gui{

  class monitor {

    friend class window;

    class Impl;
    Impl* pImpl;

    explicit monitor(Impl* pImpl) noexcept : pImpl(pImpl){}
  public:
    monitor(const monitor &) = default;
    monitor(monitor &&) noexcept = default;
    monitor & operator=(const monitor &) = default;
    monitor & operator=(monitor &&) noexcept = default;

    bool                      isPrimary() const noexcept;
    u32                       width() const noexcept;
    u32                       height() const noexcept;
    Extent2D<>                size() const noexcept;
    Position2D<i32>           position() const noexcept;
    u32                       frequency() const noexcept;
    std::string_view          name() const noexcept;
    std::string_view          path() const noexcept;
    //graphics::Extent2D<>      virtualExtents() const noexcept;
    //graphics::Extent2D<float> physicalExtents() const noexcept;
    //graphics::Box2D<i32>      screenArea() const noexcept;
    //float                     dpi() const noexcept;
    //graphics::PixelFormat     pixelFormat() const noexcept;


    static monitor            getPrimary() noexcept;
    static std::span<monitor> enumerate() noexcept;
  };
}

#endif//VALKYRIE_GUI_MONITOR_HPP
