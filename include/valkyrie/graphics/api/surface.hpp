//
// Created by Maxwell on 2020-11-16.
//

#ifndef VALKYRIE_GRAPHICS_API_SURFACE_HPP
#define VALKYRIE_GRAPHICS_API_SURFACE_HPP

#include <valkyrie/graphics/api/vulkan.hpp>

namespace valkyrie::graphics::api{
  class surface {
    class Impl;
    Impl* pImpl;
  public:
    surface(const instance & instance, Extent2D<> extent) noexcept;
    surface(const surface &);
    surface(surface &&) noexcept;
    surface & operator=(const surface &);
    surface & operator=(surface &&) noexcept;
    ~surface();




    bool isValid() const noexcept;
    Extent2D<> getExtents() const noexcept;
  };
}

#endif//VALKYRIE_GRAPHICS_API_SURFACE_HPP
