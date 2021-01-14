//
// Created by Maxwell on 2020-11-16.
//

#ifndef VALKYRIE_SURFACE_HPP
#define VALKYRIE_SURFACE_HPP

#include <valkyrie/Graphics/API/Vulkan.hpp>

namespace valkyrie::Graphics::API{
  class Surface{
    class Impl;
    Impl* pImpl;
  public:

    Surface(const Instance& instance, Core::Extent2D<> extent) noexcept;
    Surface(const Surface&);
    Surface(Surface&&) noexcept;
    Surface& operator=(const Surface&);
    Surface& operator=(Surface&&) noexcept;
    ~Surface();




    bool isValid() const noexcept;
    Core::Extent2D<> getExtents() const noexcept;
  };
}

#endif//VALKYRIE_SURFACE_HPP
