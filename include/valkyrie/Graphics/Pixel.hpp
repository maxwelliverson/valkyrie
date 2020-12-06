//
// Created by Maxwell on 2020-11-30.
//

#ifndef VALKYRIE_GRAPHICS_PIXEL_HPP
#define VALKYRIE_GRAPHICS_PIXEL_HPP


#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Types.hpp>

#include <valkyrie/Graphics/API/Vulkan.hpp>

namespace valkyrie::Graphics{


  enum class ColorSpace : u32{
    SRGBNonlinear = 0,
    DisplayP3Nonlinear = 1000104001,
    ExtendedSRBGLinear = 1000104002,
    DisplayP3Linear = 1000104003,
    DCIP3Nonlinear = 1000104004,
    BT709Linear = 1000104005,
    BT709Nonlinear = 1000104006,
    BT2020Linear = 1000104007,
    HDR10ST2084 = 1000104008,
    DolbyVision = 1000104009,
    HDR10HLG = 1000104010,
    AdobeRGBLinear = 1000104011,
    AdobeRGBNonlinear = 1000104012,
    PassThrough = 1000104013,
    ExtendedSRGBNonlinear = 1000104014,
    DisplayNativeAMD = 1000213000
  };
  /*struct PixelFormat{
    struct {
      u8 red;
      u8 green;
      u8 blue;
      u8 alpha;
    } bits;
    ColorSpace colorSpace;
    bool enableAlpha;
    bool alphaBefore;
  };*/

  namespace Formats{

    struct Any8Bit{
      u8 : 0;
    };
    struct Any16Bit{
      u16 : 0;
    };
    struct Any32Bit{
      u32 : 0;
    };
    struct Any64Bit{
      u64 : 0;
    };
    struct Any96Bit{
      u32 : 0;
      u32 : 0;
      u32 : 0;
    };
    struct Any128Bit{
      u64 : 0;
      u64 : 0;
    };
    struct Any182Bit{
      u64 : 0;
      u64 : 0;
      u64 : 0;
    };
    struct Any256Bit{
      u64 : 0;
      u64 : 0;
      u64 : 0;
      u64 : 0;
    };


    
    // Sub-byte channels
    struct R4G4UNorm{
      u8 r : 4;
      u8 g : 4;
    };
    struct R4G4B4A4UNormPack16{
      u16 r : 4;
      u16 g : 4;
      u16 b : 4;
      u16 a : 4;
    };
    struct B4G4R4A4UNormPack16{
      u16 b : 4;
      u16 g : 4;
      u16 r : 4;
      u16 a : 4;
    };
    struct A4R4G4B4UNormPack16EXT{
      u16 a : 4;
      u16 r : 4;
      u16 g : 4;
      u16 b : 4;
    };
    struct A4B4G4R4UNormPack16EXT{
      u16 a : 4;
      u16 b : 4;
      u16 g : 4;
      u16 r : 4;
    };
    struct R5G6B5UNormPack16{
      u16 r : 5;
      u16 g : 6;
      u16 b : 5;
    };
    struct B5G6R5UNormPack16{
      u16 b : 5;
      u16 g : 6;
      u16 r : 5;
    };
    struct R5G5B5A1UNormPack16{
      u16 r : 5;
      u16 g : 5;
      u16 b : 5;
      u16 a : 1;
    };
    struct B5G5R5A1UNormPack16{
      u16 b : 5;
      u16 g : 5;
      u16 r : 5;
      u16 a : 1;
    };
    struct A1R5G5B5UNormPack16{
      u16 a : 1;
      u16 r : 5;
      u16 g : 5;
      u16 b : 5;
    };
    
    
    
    
    // 8 bit channels
    struct R8UNorm{
      u8 r;
    };
    struct R8SNorm{
      i8 r;
    };
    struct R8UScaled{
      u8 r;
    };
    struct R8SScaled{
      i8 r;
    };
    struct R8UInt{
      u8 r;
    };
    struct R8SInt{
      i8 r;
    };
    struct R8SRGB{
      u8 r;
    };
    
    struct R8G8UNorm{
      u8 r;
      u8 g;
    };
    struct R8G8SNorm{
      i8 r;
      i8 g;
    };
    struct R8G8UScaled{
      u8 r;
      u8 g;
    };
    struct R8G8SScaled{
      i8 r;
      i8 g;
    };
    struct R8G8UInt{
      u8 r;
      u8 g;
    };
    struct R8G8SInt{
      i8 r;
      i8 g;
    };
    struct R8G8SRGB{
      u8 r;
      u8 g;
    };
    
    struct R8G8B8UNorm{
      u8 r;
      u8 g;
      u8 b;
    };
    struct R8G8B8SNorm{
      i8 r;
      i8 g;
      i8 b;
    };
    struct R8G8B8UScaled{
      u8 r;
      u8 g;
      u8 b;
    };
    struct R8G8B8SScaled{
      i8 r;
      i8 g;
      i8 b;
    };
    struct R8G8B8UInt{
      u8 r;
      u8 g;
      u8 b;
    };
    struct R8G8B8SInt{
      i8 r;
      i8 g;
      i8 b;
    };
    struct R8G8B8SRGB{
      u8 r;
      u8 g;
      u8 b;
    };

    struct B8G8R8UNorm{
      u8 b;
      u8 g;
      u8 r;
    };
    struct B8G8R8SNorm{
      i8 b;
      i8 g;
      i8 r;
    };
    struct B8G8R8UScaled{
      u8 b;
      u8 g;
      u8 r;
    };
    struct B8G8R8SScaled{
      i8 b;
      i8 g;
      i8 r;
    };
    struct B8G8R8UInt{
      u8 b;
      u8 g;
      u8 r;
    };
    struct B8G8R8SInt{
      i8 b;
      i8 g;
      i8 r;
    };
    struct B8G8R8SRGB{
      u8 b;
      u8 g;
      u8 r;
    };
    
    struct R8G8B8A8UNorm{
      u8 r;
      u8 g;
      u8 b;
      u8 a;
    };
    struct R8G8B8A8SNorm{
      i8 r;
      i8 g;
      i8 b;
      i8 a;
    };
    struct R8G8B8A8UScaled{
      u8 r;
      u8 g;
      u8 b;
      u8 a;
    };
    struct R8G8B8A8SScaled{
      i8 r;
      i8 g;
      i8 b;
      i8 a;
    };
    struct R8G8B8A8UInt{
      u8 r;
      u8 g;
      u8 b;
      u8 a;
    };
    struct R8G8B8A8SInt{
      i8 r;
      i8 g;
      i8 b;
      i8 a;
    };
    struct R8G8B8A8SRGB{
      u8 r;
      u8 g;
      u8 b;
      u8 a;
    };

    struct B8G8R8A8UNorm{
      u8 b;
      u8 g;
      u8 r;
      u8 a;
    };
    struct B8G8R8A8SNorm{
      i8 b;
      i8 g;
      i8 r;
      i8 a;
    };
    struct B8G8R8A8UScaled{
      u8 b;
      u8 g;
      u8 r;
      u8 a;
    };
    struct B8G8R8A8SScaled{
      i8 b;
      i8 g;
      i8 r;
      i8 a;
    };
    struct B8G8R8A8UInt{
      u8 b;
      u8 g;
      u8 r;
      u8 a;
    };
    struct B8G8R8A8SInt{
      i8 b;
      i8 g;
      i8 r;
      i8 a;
    };
    struct B8G8R8A8SRGB{
      u8 b;
      u8 g;
      u8 r;
      u8 a;
    };

    struct A8B8G8R8UNormPack32{
      u8 a;
      u8 b;
      u8 g;
      u8 r;
    };
    struct A8B8G8R8SNormPack32{
      i8 a;
      i8 b;
      i8 g;
      i8 r;
    };
    struct A8B8G8R8UScaledPack32{
      u8 a;
      u8 b;
      u8 g;
      u8 r;
    };
    struct A8B8G8R8SScaledPack32{
      i8 a;
      i8 b;
      i8 g;
      i8 r;
    };
    struct A8B8G8R8UIntPack32{
      u8 a;
      u8 b;
      u8 g;
      u8 r;
    };
    struct A8B8G8R8SIntPack32{
      i8 a;
      i8 b;
      i8 g;
      i8 r;
    };
    struct A8B8G8R8SRGBPack32{
      u8 a;
      u8 b;
      u8 g;
      u8 r;
    };
    
    
    // 10 bit channels
    
    struct A2R10G10B10UNormPack32{
      u32 a : 2;
      u32 r : 10;
      u32 g : 10;
      u32 b : 10;
    };
    struct A2R10G10B10SNormPack32{
      i32 a : 2;
      i32 r : 10;
      i32 g : 10;
      i32 b : 10;
    };
    struct A2R10G10B10UScaledPack32{
      u32 a : 2;
      u32 r : 10;
      u32 g : 10;
      u32 b : 10;
    };
    struct A2R10G10B10SScaledPack32{
      i32 a : 2;
      i32 r : 10;
      i32 g : 10;
      i32 b : 10;
    };
    struct A2R10G10B10UIntPack32{
      u32 a : 2;
      u32 r : 10;
      u32 g : 10;
      u32 b : 10;
    };
    struct A2R10G10B10SIntPack32{
      i32 a : 2;
      i32 r : 10;
      i32 g : 10;
      i32 b : 10;
    };

    struct A2B10G10R10UNormPack32{
      u32 a : 2;
      u32 b : 10;
      u32 g : 10;
      u32 r : 10;
    };
    struct A2B10G10R10SNormPack32{
      i32 a : 2;
      i32 b : 10;
      i32 g : 10;
      i32 r : 10;
    };
    struct A2B10G10R10UScaledPack32{
      u32 a : 2;
      u32 b : 10;
      u32 g : 10;
      u32 r : 10;
    };
    struct A2B10G10R10SScaledPack32{
      i32 a : 2;
      i32 b : 10;
      i32 g : 10;
      i32 r : 10;
    };
    struct A2B10G10R10UIntPack32{
      u32 a : 2;
      u32 b : 10;
      u32 g : 10;
      u32 r : 10;
    };
    struct A2B10G10R10SIntPack32{
      i32 a : 2;
      i32 b : 10;
      i32 g : 10;
      i32 r : 10;
    };
    
    
    // 16 bit channels
    
    struct R16UNorm{
      u16 r;
    };
    struct R16SNorm{
      i16 r;
    };
    struct R16UScaled{
      u16 r;
    };
    struct R16SScaled{
      i16 r;
    };
    struct R16UInt{
      u16 r;
    };
    struct R16SInt{
      i16 r;
    };
    struct R16SFloat{};
    
    struct R16G16UNorm{
      u16 r;
      u16 g;
    };
    struct R16G16SNorm{
      i16 r;
      i16 g;
    };
    struct R16G16UScaled{
      u16 r;
      u16 g;
    };
    struct R16G16SScaled{
      i16 r;
      i16 g;
    };
    struct R16G16UInt{
      u16 r;
      u16 g;
    };
    struct R16G16SInt{
      i16 r;
      i16 g;
    };
    struct R16G16SFloat{};
    
    struct R16G16B16UNorm{
      u16 r;
      u16 g;
      u16 b;
    };
    struct R16G16B16SNorm{
      i16 r;
      i16 g;
      i16 b;
    };
    struct R16G16B16UScaled{
      u16 r;
      u16 g;
      u16 b;
    };
    struct R16G16B16SScaled{
      i16 r;
      i16 g;
      i16 b;
    };
    struct R16G16B16UInt{
      u16 r;
      u16 g;
      u16 b;
    };
    struct R16G16B16SInt{
      i16 r;
      i16 g;
      i16 b;
    };
    struct R16G16B16SFloat{};
    
    struct R16G16B16A16UNorm{
      u16 r;
      u16 g;
      u16 b;
      u16 a;
    };
    struct R16G16B16A16SNorm{
      i16 r;
      i16 g;
      i16 b;
      i16 a;
    };
    struct R16G16B16A16UScaled{
      u16 r;
      u16 g;
      u16 b;
      u16 a;
    };
    struct R16G16B16A16SScaled{
      i16 r;
      i16 g;
      i16 b;
      i16 a;
    };
    struct R16G16B16A16UInt{
      u16 r;
      u16 g;
      u16 b;
      u16 a;
    };
    struct R16G16B16A16SInt{
      i16 r;
      i16 g;
      i16 b;
      i16 a;
    };
    struct R16G16B16A16SFloat{};
    
    
    // 32 bit channels
    
    struct R32UInt{
      u32 r;
    };
    struct R32SInt{
      i32 r;
    };
    struct R32SFloat{};
    
    struct R32G32UInt{
      u32 r;
      u32 g;
    };
    struct R32G32SInt{
      i32 r;
      i32 g;
    };
    struct R32G32SFloat{};
    
    struct R32G32B32UInt{
      u32 r;
      u32 g;
      u32 b;
    };
    struct R32G32B32SInt{
      i32 r;
      i32 g;
      i32 b;
    };
    struct R32G32B32SFloat{};
    
    struct R32G32B32A32UInt{
      u32 r;
      u32 g;
      u32 b;
      u32 a;
    };
    struct R32G32B32A32SInt{
      i32 r;
      i32 g;
      i32 b;
      i32 a;
    };
    struct R32G32B32A32SFloat{};
    
    
    // 64 bit channels
    
    struct R64UInt{
      u64 r;
    };
    struct R64SInt{
      i64 r;
    };
    struct R64SFloat{};
    
    struct R64G64UInt{
      u64 r;
      u64 g;
    };
    struct R64G64SInt{
      i64 r;
      i64 g;
    };
    struct R64G64SFloat{};
    
    struct R64G64B64UInt{
      u64 r;
      u64 g;
      u64 b;
    };
    struct R64G64B64SInt{
      i64 r;
      i64 g;
      i64 b;
    };
    struct R64G64B64SFloat{};
    
    struct R64G64B64A64UInt{
      u64 r;
      u64 g;
      u64 b;
      u64 a;
    };
    struct R64G64B64A64SInt{
      i64 r;
      i64 g;
      i64 b;
      i64 a;
    };
    struct R64G64B64A64SFloat{};
    
    
    // Others
    
    struct B10G11R11UFloatPack32{
      u32 b : 10;
      u32 g : 11;
      u32 r : 11;
    };
    struct E5B9G9R9UFloatPack32{
      u32 exp : 5;
      u32 b   : 9;
      u32 g   : 9;
      u32 r   : 9;
    };
    struct R12X4UNormPack16{
      u16 r : 12;
      u16 : 0;
    };
    struct R10X6UNormPack16{
      u16 r : 10;
      u16 : 0;
    };
    struct R10X6G10X6UNorm_2PACK16{
      u16 r : 10;
      u16 : 0;
      u16 g : 10;
      u16 : 0;
    };
    struct R10X6G10X6B10X6A10X6UNorm_4PACK16{
      u16 r : 10;
      u16 : 0;
      u16 g : 10;
      u16 : 0;
      u16 b : 10;
      u16 : 0;
      u16 a : 10;
      u16 : 0;
    };

    
    
    struct UInt{};
    struct SInt{};
    struct UNorm{};
    struct SNorm{};
    struct UScaled{};
    struct SScaled{};
    struct SRGB{};
    struct SFloat{};
    struct UFloat{};
    
    inline constexpr static API::ComponentSwizzle R = API::ComponentSwizzle::R;
    inline constexpr static API::ComponentSwizzle G = API::ComponentSwizzle::G;
    inline constexpr static API::ComponentSwizzle B = API::ComponentSwizzle::B;
    inline constexpr static API::ComponentSwizzle A = API::ComponentSwizzle::A;


    template <typename P>
    struct FormatProperties;
    
    
    
    
    template <size_t N>
    struct ComponentOrder;
    template <>
    struct ComponentOrder<1>{
      API::ComponentSwizzle r;
      inline constexpr static API::ComponentSwizzle g = API::ComponentSwizzle::Zero;
      inline constexpr static API::ComponentSwizzle b = API::ComponentSwizzle::Zero;
      inline constexpr static API::ComponentSwizzle a = API::ComponentSwizzle::One;
    };
    template <>
    struct ComponentOrder<2>{
      API::ComponentSwizzle r;
      API::ComponentSwizzle g;
      inline constexpr static API::ComponentSwizzle b = API::ComponentSwizzle::Zero;
      inline constexpr static API::ComponentSwizzle a = API::ComponentSwizzle::One;
    };
    template <>
    struct ComponentOrder<3>{
      API::ComponentSwizzle r;
      API::ComponentSwizzle g;
      API::ComponentSwizzle b;
      inline constexpr static API::ComponentSwizzle a = API::ComponentSwizzle::One;
    };
    template <>
    struct ComponentOrder<4>{
      API::ComponentSwizzle r;
      API::ComponentSwizzle g;
      API::ComponentSwizzle b;
      API::ComponentSwizzle a;
    };
    
    template <typename ...T>
    ComponentOrder(T...) -> ComponentOrder<sizeof...(T)>;

#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_8_2(a, b, c, d, e, f, g, h) a##b##c##d##e##f##g##h
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_8 PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_8_2
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_8(...) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_8(__VA_ARGS__)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_7_2(a, b, c, d, e, f, g) a##b##c##d##e##f##g
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_7 PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_7_2
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_7(...) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_7(__VA_ARGS__)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_6_2(a, b, c, d, e, f) a##b##c##d##e##f
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_6 PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_6_2
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_6(...) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_6(__VA_ARGS__)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_5_2(a, b, c, d, e) a##b##c##d##e
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_5 PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_5_2
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_5(...) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_5(__VA_ARGS__)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_4_2(a, b, c, d) a##b##c##d
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_4 PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_4_2
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_4(...) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_4(__VA_ARGS__)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_3_2(a, b, c) a##b##c
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_3 PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_3_2
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_3(...) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_3(__VA_ARGS__)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_2_2(a, b) a##b
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_2 PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_2_2
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_2(...) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_redirect_2(__VA_ARGS__)


#define PP_VK_impl_SMALL_COUNT_GET_9TH(a, b, c, d, e, f, g, h, i, j, ...) j
#define PP_VK_impl_SMALL_COUNT_redirect(...) PP_VK_impl_SMALL_COUNT_GET_9TH(, ##__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define PP_VK_impl_SMALL_COUNT PP_VK_impl_SMALL_COUNT_redirect
#define VK_small_count(...) PP_VK_impl_SMALL_COUNT(__VA_ARGS__)


#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_X
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_A A
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_R R
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_G G
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_B B
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA_X
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA_A ,
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA_R ,
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA_G ,
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA_B ,
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_RETAIN_COMMA_
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_RETAIN_COMMA_A  A
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_RETAIN_COMMA_R  R
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_RETAIN_COMMA_G  G
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_RETAIN_COMMA_B  B

#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_ONE  API::ComponentSwizzle::One
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_ZERO API::ComponentSwizzle::Zero

#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_RETAIN_COMMA_redirect(...) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_RETAIN_COMMA_##__VA_ARGS__
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_RETAIN_COMMA(...) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_RETAIN_COMMA_redirect(__VA_ARGS__)

#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_ONLY(a, b) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_##a
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA(a, b) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_##a PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA_##a
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_LAST(a, b) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_RETAIN_COMMA(PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_##a)
    
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_8(a, b, c, d, e, f, g, h) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA a                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA b                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA c                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA d                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA e                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA f                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA g                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_LAST h
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_7(a, b, c, d, e, f, g) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA a                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA b                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA c                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA d                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA e                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA f                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_LAST g 
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_6(a, b, c, d, e, f) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA a                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA b                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA c                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA d                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA e                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_LAST f 
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_5(a, b, c, d, e) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA a                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA b                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA c                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA d                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_LAST e 
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_4(a, b, c, d) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA a                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA b                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA c                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_LAST d
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_3(a, b, c) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA a                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA b                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_LAST c 
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_2(a, b) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_COMMA a                       \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_LAST b 
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_1(a) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_ONLY a
    

#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_DISPATCH(...) VK_concat(PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_, VK_small_count(__VA_ARGS__))(__VA_ARGS__)

#define PP_VK_impl_DEFINE_FORMAT_OUTER_CONCAT_redirect_2(Layout, Suffix) Layout##Suffix
#define PP_VK_impl_DEFINE_FORMAT_OUTER_CONCAT_redirect PP_VK_impl_DEFINE_FORMAT_OUTER_CONCAT_redirect_2
#define PP_VK_impl_DEFINE_FORMAT_OUTER_CONCAT(...) PP_VK_impl_DEFINE_FORMAT_OUTER_CONCAT_redirect(__VA_ARGS__)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME(Layout, Type) PP_VK_impl_DEFINE_FORMAT_OUTER_CONCAT(VK_concat(PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_, VK_small_count Layout) Layout, Type)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_8(a, b, c, d, e, f, g, h) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_8(VK_concat_inner a, VK_concat_inner b, VK_concat_inner c, VK_concat_inner d, VK_concat_inner e, VK_concat_inner f, VK_concat_inner g, VK_concat_inner h)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_7(a, b, c, d, e, f, g) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_7(VK_concat_inner a, VK_concat_inner b, VK_concat_inner c, VK_concat_inner d, VK_concat_inner e, VK_concat_inner f, VK_concat_inner g)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_6(a, b, c, d, e, f) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_6(VK_concat_inner a, VK_concat_inner b, VK_concat_inner c, VK_concat_inner d, VK_concat_inner e, VK_concat_inner f)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_5(a, b, c, d, e) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_5(VK_concat_inner a, VK_concat_inner b, VK_concat_inner c, VK_concat_inner d, VK_concat_inner e)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_4(a, b, c, d) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_4(VK_concat_inner a, VK_concat_inner b, VK_concat_inner c, VK_concat_inner d)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_3(a, b, c) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_3(VK_concat_inner a, VK_concat_inner b, VK_concat_inner c)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_2(a, b) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_CONCAT_2(VK_concat_inner a, VK_concat_inner b)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME_1(a) VK_concat_inner a
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_X(Num)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_A(Num) inline constexpr static auto alpha_bits = Num;
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_R(Num) inline constexpr static auto   red_bits = Num;
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_G(Num) inline constexpr static auto green_bits = Num;
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_B(Num) inline constexpr static auto  blue_bits = Num;
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY(a, b) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_##a(b)


#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_8(a, b, c, d, e, f, g, h) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY a \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY b \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY c \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY d \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY e \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY f \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY g \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY h
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_7(a, b, c, d, e, f, g) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY a \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY b \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY c \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY d \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY e \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY f \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY g
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_6(a, b, c, d, e, f)  \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY a \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY b \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY c \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY d \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY e \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY f
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_5(a, b, c, d, e) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY a \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY b \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY c \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY d \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY e
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_4(a, b, c, d) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY a \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY b \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY c \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY d
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_3(a, b, c) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY a \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY b \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY c
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_2(a, b) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY a \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY b
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_1(a) \
    PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_APPLY a

#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_DISPATCH_redirect_2(Suf, Layout) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_##Suf Layout
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_DISPATCH_redirect PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_DISPATCH_redirect_2
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_DISPATCH_first_layer(...) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_DISPATCH_redirect(__VA_ARGS__)
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_DISPATCH(Layout) PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_DISPATCH_first_layer(VK_small_count Layout, Layout)


#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES_WITH_SUFFIX(Layout, Type, Suffix) template <>\
    struct FormatProperties<PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME(Layout, Type##Suffix)>{ \
       PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_DISPATCH(Layout)        \
                                                                             \
       inline constexpr static ComponentOrder ordering{ PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_DISPATCH Layout }; \
                                                                             \
       using component_type = Type;                                          \
       inline constexpr static auto vulkan_format = API::Format:: PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME(Layout, Type##Suffix); \
    };
#define PP_VK_impl_DEFINE_FORMAT_PROPERTIES(Layout, Type) template <>\
    struct FormatProperties<PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME(Layout, Type)>{ \
       PP_VK_impl_DEFINE_FORMAT_PROPERTIES_BITS_COUNT_DISPATCH(Layout)       \
                                                                             \
       inline constexpr static ComponentOrder ordering{ PP_VK_impl_DEFINE_FORMAT_PROPERTIES_ORDERING_DISPATCH Layout }; \
                                                                             \
       using component_type = Type;                                          \
       inline constexpr static auto vulkan_format = API::Format:: PP_VK_impl_DEFINE_FORMAT_PROPERTIES_TYPENAME(Layout, Type); \
    };

#define VK_define_format_family(Layout, ...) VK_foreach(VK_bind_front(PP_VK_impl_DEFINE_FORMAT_PROPERTIES, Layout), __VA_ARGS__)
#define VK_define_packed_format_family(Layout, Packing, ...) VK_foreach(VK_bind_back(VK_bind_front(PP_VK_impl_DEFINE_FORMAT_PROPERTIES_WITH_SUFFIX, Layout), Packing), __VA_ARGS__)
  
  
    VK_define_format_family(((R, 8)),                         UNorm, SNorm, UScaled, SScaled, UInt, SInt, SRGB)
    VK_define_format_family(((R, 8), (G, 8)),                 UNorm, SNorm, UScaled, SScaled, UInt, SInt, SRGB)
    VK_define_format_family(((R, 8), (G, 8), (B, 8)),         UNorm, SNorm, UScaled, SScaled, UInt, SInt, SRGB)
    VK_define_format_family(((B, 8), (G, 8), (R, 8)),         UNorm, SNorm, UScaled, SScaled, UInt, SInt, SRGB)
    VK_define_format_family(((R, 8), (G, 8), (B, 8), (A, 8)), UNorm, SNorm, UScaled, SScaled, UInt, SInt, SRGB)
    VK_define_format_family(((B, 8), (G, 8), (R, 8), (A, 8)), UNorm, SNorm, UScaled, SScaled, UInt, SInt, SRGB)
    
    VK_define_packed_format_family(((A, 8), (B, 8),  (G, 8),  (R, 8)), Pack32, UNorm, SNorm, UScaled, SScaled, UInt, SInt, SRGB)
    
    VK_define_packed_format_family(((A, 2), (R, 10), (G, 10), (B, 10)), Pack32, UNorm, SNorm, UScaled, SScaled, UInt, SInt)
    VK_define_packed_format_family(((A, 2), (B, 10), (G, 10), (R, 10)), Pack32, UNorm, SNorm, UScaled, SScaled, UInt, SInt)
    
    VK_define_format_family(((R, 16)),                            UNorm, SNorm, UScaled, SScaled, UInt, SInt, SFloat)
    VK_define_format_family(((R, 16), (G, 16)),                   UNorm, SNorm, UScaled, SScaled, UInt, SInt, SFloat)
    VK_define_format_family(((R, 16), (G, 16), (B, 16)),          UNorm, SNorm, UScaled, SScaled, UInt, SInt, SFloat)
    VK_define_format_family(((R, 16), (G, 16), (B, 16), (A, 16)), UNorm, SNorm, UScaled, SScaled, UInt, SInt, SFloat)
  
    VK_define_format_family(((R, 32)),                            UInt, SInt, SFloat)
    VK_define_format_family(((R, 32), (G, 32)),                   UInt, SInt, SFloat)
    VK_define_format_family(((R, 32), (G, 32), (B, 32)),          UInt, SInt, SFloat)
    VK_define_format_family(((R, 32), (G, 32), (B, 32), (A, 32)), UInt, SInt, SFloat)


    VK_define_format_family(((R, 64)),                            UInt, SInt, SFloat)
    VK_define_format_family(((R, 64), (G, 64)),                   UInt, SInt, SFloat)
    VK_define_format_family(((R, 64), (G, 64), (B, 64)),          UInt, SInt, SFloat)
    VK_define_format_family(((R, 64), (G, 64), (B, 64), (A, 64)), UInt, SInt, SFloat)
    
    template <typename T>
    struct GreenBits{
      inline constexpr static auto green_bits = 0;
    };
    template <typename T> requires(requires{ { FormatProperties<T>::green_bits }; })
    struct GreenBits<T>{
      inline constexpr static auto green_bits = FormatProperties<T>::green_bits;
    };
    template <typename T>
    struct BlueBits{
      inline constexpr static auto blue_bits = 0;
    };
    template <typename T> requires(requires{ { FormatProperties<T>::blue_bits }; })
    struct BlueBits<T>{
      inline constexpr static auto blue_bits = FormatProperties<T>::blue_bits;
    };
    template <typename T>
    struct AlphaBits{
      inline constexpr static auto alpha_bits = 0;
    };
    template <typename T> requires(requires{ { FormatProperties<T>::alpha_bits }; })
    struct AlphaBits<T>{
      inline constexpr static auto alpha_bits = FormatProperties<T>::alpha_bits;
    };



    template <size_t N>
    struct ErasedFormat;
    template <typename T>
    struct IsErasedFormat : std::false_type{};

#define PP_VK_impl_DEF_ERASED_FORMAT(N) template <> struct ErasedFormat<N>{ using type = Any##N##Bit; }; template <> struct IsErasedFormat<Any##N##Bit> : std::true_type{};

    VK_foreach(PP_VK_impl_DEF_ERASED_FORMAT, 8, 16, 32, 64, 96, 128, 182, 256)

    template <typename T>
    struct FormatTraits
        : GreenBits<T>,
          BlueBits<T>,
          AlphaBits<T>{
      inline constexpr static auto red_bits   = FormatProperties<T>::red_bits;

      inline constexpr static auto bits_per_pixel = red_bits + GreenBits<T>::green_bits + BlueBits<T>::blue_bits + AlphaBits<T>::alpha_bits;
      inline constexpr static auto size           = bits_per_pixel >> 3;

      inline constexpr static decltype(auto) ordering      = FormatProperties<T>::ordering;
      inline constexpr static decltype(auto) vulkan_format = FormatProperties<T>::vulkan_format;

      inline constexpr static bool is_typed = true;

      using component_type = typename FormatProperties<T>::component_type;
      using storage_type   = typename ErasedFormat<size>::type;
    };
    template <typename T> requires(IsErasedFormat<T>::value)
    struct FormatTraits<T>{
      inline constexpr static bool is_typed = false;
      inline constexpr static size_t size = sizeof(T);
      inline constexpr static size_t bits_per_pixel = size << 8;

    };



  };

  template <typename T>
  using format_traits = Formats::FormatTraits<T>;


  template <typename PixelFormat>
  class Pixel{

  };
  template <typename F>
  requires(!format_traits<F>::is_typed)
  class Pixel<F>{

  };

};

#endif//VALKYRIE_GRAPHICS_PIXEL_HPP
