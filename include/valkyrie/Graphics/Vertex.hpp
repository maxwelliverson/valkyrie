//
// Created by Maxwell on 2020-11-16.
//

#ifndef VALKYRIE_GRAPHICS_VERTEX_HPP
#define VALKYRIE_GRAPHICS_VERTEX_HPP

#include <valkyrie/Core/Types.hpp>

namespace valkyrie::Graphics{
  struct Vertex{
    struct {
      f32 x, y;
    } pos;
    struct {
      f32 r, g, b;
    } color;
  };
}

#endif//VALKYRIE_GRAPHICS_VERTEX_HPP
