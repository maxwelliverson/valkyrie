//
// Created by Maxwell on 2020-11-30.
//

#ifndef VALKYRIE_GRAPHICS_TEXEL_HPP
#define VALKYRIE_GRAPHICS_TEXEL_HPP

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Types.hpp>

namespace valkyrie::Graphics{
  enum class TexelComponentOrder{
    ARGB,
    ARBG,
    ABRG,
    ABGR,
    AGBR,
    AGRB,
    ARG,
    AGR,
    ARB,
    ABR,
    ABG,
    AGB,
    AR,
    AB,
    AG,
    A,
    RGBA,
    RGAB,
    RBGA,
    RBAG,
    RABG,
    RAGB,
    RAG,
    RGA,
    RAB,
    RBA,
    RBG,
    RGB,
    RA,
    RB,
    RG,
    R,
    BRGA,
    BRAG,
    BGRA,
    BGAR,
    BARG,
    BAGR,
    BGA,
    BAG,
    BRA,
    BAR,
    BRG,
    BGR,
    BA,
    BG,
    BR,
    B,
    GBRA,
    GBAR,
    GRBA,
    GRAB,
    GARB,
    GABR,
    GRA,
    GAR,
    GBA,
    GAB,
    GBR,
    GRB,
    GA,
    GB,
    GR,
    G
  };
  enum class TexelComponentType : u8{
    Int,
    UnsignedInt,
    Float,
    UnsignedFloat,
    Normalized,
    UnsignedNormalized,
    ScaledInt,
    UnsignedScaledInt
    SRGB,
    None
  };
  enum class TexelKind{
    Pixel,
    DepthStencil,
    BC1,
    BC2,
    BC3,
    BC4,
    BC5,
    BC6H,
    BC7,
    ASTC,
    ETC2,
    PVRTC
  };


  struct TexelFormat{
    union{
      struct {
        u8 red;
        u8 green;
        u8 blue;
        u8 alpha;
      };
      struct {
        u16 depth;
        u16 stencil;
      };
      u32 block;
    } bits;
    struct {
      TexelComponentOrder order;
      TexelComponentType type;
      TexelComponentType secondaryType;
      TexelKind kind;
    } component;
  };
  class Texel{};
}

#endif//VALKYRIE_GRAPHICS_TEXEL_HPP
