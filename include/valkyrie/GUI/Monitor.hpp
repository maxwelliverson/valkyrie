//
// Created by Maxwell on 2020-11-23.
//

#ifndef VALKYRIE_MONITOR_HPP
#define VALKYRIE_MONITOR_HPP

#include <valkyrie/Graphics/Pixel.hpp>
#include <valkyrie/Graphics/Shapes.hpp>
#include <valkyrie/Core/Error/Status.hpp>

namespace valkyrie::GUI{

  enum class Unit{
    Pixel,
    Inch,
    Centimetre,
    Millimetre
  };

  class Monitor{

    friend class Window;

    class Impl;
    Impl* pImpl;

    explicit Monitor(Impl* pImpl) noexcept : pImpl(pImpl){}
  public:
    Monitor(const Monitor&) = default;
    Monitor(Monitor&&) noexcept = default;
    Monitor& operator=(const Monitor&) = default;
    Monitor& operator=(Monitor&&) noexcept = default;

    bool                      isPrimary() const noexcept;
    u32                       width() const noexcept;
    u32                       height() const noexcept;
    Graphics::Position2D<i32> position() const noexcept;
    u32                       frequency() const noexcept;
    std::string_view          name() const noexcept;
    std::string_view          path() const noexcept;
    //Graphics::Extent2D<>      virtualExtents() const noexcept;
    //Graphics::Extent2D<float> physicalExtents() const noexcept;
    //Graphics::Box2D<i32>      screenArea() const noexcept;
    //float                     dpi() const noexcept;
    //Graphics::PixelFormat     pixelFormat() const noexcept;


    static Monitor            getPrimary() noexcept;
    static std::span<Monitor> enumerate() noexcept;
  };
}

#endif//VALKYRIE_MONITOR_HPP
