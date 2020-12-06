//
// Created by Maxwell on 2020-11-12.
//

#ifndef VALKYRIE_PIPELINE_HPP
#define VALKYRIE_PIPELINE_HPP

#include <valkyrie/Graphics/API/Vulkan.hpp>
#include <valkyrie/Graphics/Shaders.hpp>

#include <valkyrie/Core/Utility/FlatMap.hpp>

#include <optional>

namespace valkyrie::Graphics::API {
  class Pipeline : public VulkanType {
    class Impl;
    Impl* pImpl;
  public:

  };

  enum class PrimitiveTopology{
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
    TriangleFan,
    LineListWithAdjacency,
    LineStripWithAdjacency,
    TriangleListWithAdjacency,
    TriangleStripWithAdjacency,
    PatchList
  };
  enum class PolygonMode{
    Fill,
    Line,
    Point,
    FillRectangleNV = 1000153000
  };
  enum class CullModeBits{
    None = 0x0,
    Front = 0x1,
    Back = 0x2,
    FrontAndBack = 0x3
  };
  enum class FrontFace{
    CounterClockwise,
    Clockwise
  };
  enum class SampleCount{
    x1 = 0x1,
    x2 = 0x2,
    x4 = 0x4,
    x8 = 0x8,
    x16 = 0x10,
    x32 = 0x20,
    x64 = 0x40
  };
  enum class CompareOp{
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always
  };
  enum class StencilOp{
    Keep,
    Zero,
    Replace,
    IncrementAndClamp,
    DecrementAndClamp,
    Invert,
    IncrementAndWrap,
    DecrementAndWrap
  };
  enum class BlendFactor{
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstDolor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha,
    SrcAlphaSaturate,
    Src1Color,
    OneMinusSrc1Color,
    Src1Alpha,
    OneMinusSr1cAlpha
  };
  enum class BlendOp{
    Add = 0,
    Subtract = 1,
    ReverseSubtract = 2,
    Min = 3,
    Max = 4,

    // Provided by VK_EXT_blend_operation_advanced

    Zero = 1000148000,
    Src = 1000148001,
    Dst = 1000148002,
    SrcOver = 1000148003,
    DstOver = 1000148004,
    SrcIn = 1000148005,
    DstIn = 1000148006,
    SrcOut = 1000148007,
    DstOut = 1000148008,
    SrcAtop = 1000148009,
    DstAtop = 1000148010,
    Xor = 1000148011,
    Multiply = 1000148012,
    Screen = 1000148013,
    Overlay = 1000148014,
    Darken = 1000148015,
    Lighten = 1000148016,
    ColorDodge = 1000148017,
    ColorBurn = 1000148018,
    Hardlight = 1000148019,
    Softlight = 1000148020,
    Difference = 1000148021,
    Exclusion = 1000148022,
    Invert = 1000148023,
    InvertRGB = 1000148024,
    LinearDodge = 1000148025,
    LinearBurn = 1000148026,
    VividLight = 1000148027,
    LinearLight = 1000148028,
    PinLight = 1000148029,
    HardMix = 1000148030,
    HslHue = 1000148031,
    HslSaturation = 1000148032,
    HslColor = 1000148033,
    HslLuminosity = 1000148034,
    Plus = 1000148035,
    PlusClamped = 1000148036,
    PlusClampedAlpha = 1000148037,
    PlusDarker = 1000148038,
    Minus = 1000148039,
    MinusClamped = 1000148040,
    Contrast = 1000148041,
    InvertOVG = 1000148042,
    Red = 1000148043,
    Green = 1000148044,
    Blue = 1000148045,
  };
  enum class ColorComponent{
    R = 0x1,
    G = 0x2,
    B = 0x4,
    A = 0x8
  };
  enum class LogicOp{
    Clear,
    And,
    AndReverse,
    Copy,
    AndInverted,
    NoOp,
    Xor,
    Or,
    Not,
    Equivalent,
    Invert,
    OrReverse,
    CopyInverted,
    OrInverted,
    Nand,
    Set
  };
  enum class BlendOverlap : u8{
    Uncorrelated,
    Disjoint,
    Conjoint
  };
  enum class DynamicStateFlags{
    Viewport = 0,
    Scissor = 1,
    LineWidth = 2,
    DepthBias = 3,
    BlendConstants = 4,
    DepthBounds = 5,
    StencilCompareMask = 6,
    StencilWriteMask = 7,
    StencilReference = 8,

    // Provided by VK_NV_clip_space_w_scaling
    ViewportWithScalingNV = 1000087000,

    // Provided by VK_EXT_discard_rectangles
    DiscardRectangle = 1000099000,

    // Provided by VK_EXT_sample_locations
    SampleLocationsEXT = 1000143000,

    // Provided by VK_KHR_ray_tracing
    RayTracingPipelineStackSizeKHR = 1000347000,

    // Provided by VK_NV_shading_rate_image
    ViewportShadingRatePaletteNV = 1000164004,
    ViewportCoarseSampleOrderNV = 1000164006,

    // Provided by VK_NV_scissor_exclusive
    ExclusiveScissorNV = 1000205001,

    // Provided by VK_KHR_fragment_shading_rate
    FragmentShadingRateKHR = 1000226000,

    // Provided by VK_EXT_line_rasterization
    LineStippleEXT = 1000259000,

    // Provided by VK_EXT_extended_dynamic_state
    CullModeEXT = 1000267000,
    FrontFaceEXT = 1000267001,
    PrimitiveTopologyEXT = 1000267002,
    ViewportWithCountEXT = 1000267003,
    ScissorWithCountEXT = 1000267004,
    VertexInputBindingStrideEXT = 1000267005,
    DepthTestEnableEXT = 1000267006,
    DepthWriteEnableEXT = 1000267007,
    DepthCompareOpEXT = 1000267008,
    DepthBoundsTestEnableEXT = 1000267009,
    StencilTestEnableEXT = 1000267010,
    StencilOpEXT = 1000267011
  };
  enum class DescriptorSetLayoutBits{
    UpdateAfterBindPool = 0x2,
    PushDescriptor      = 0x1
  };
  enum class DescriptorType{
    Sampler = 0,
    CombinedImageSampler = 1,
    SampledImage = 2,
    StorageImage = 3,
    UniformTexelBuffer = 4,
    StorageTexelBuffer = 5,
    UniformBuffer = 6,
    StorageBuffer = 7,
    UniformBufferDynamic = 8,
    StorageBufferDynamic = 9,
    InputAttachment = 10,

    // Provided by VK_EXT_inline_uniform_block
    InlineUniformBlockEXT = 1000138000,
    // Provided by VK_KHR_ray_tracing
    AccelerationStructureKHR = 1000150000,
    // Provided by VK_NV_ray_tracing
    AccelerationStructureNV = 1000165000,
  };
  enum class Format{
    Undefined = 0,
    R4G4UNormPack8 = 1,
    R4G4B4A4UNormPack16 = 2,
    B4G4R4A4UNormPack16 = 3,
    R5G6B5UNormPack16 = 4,
    B5G6R5UNormPack16 = 5,
    R5G5B5A1UNormPack16 = 6,
    B5G5R5A1UNormPack16 = 7,
    A1R5G5B5UNormPack16 = 8,
    R8UNorm = 9,
    R8SNorm = 10,
    R8UScaled = 11,
    R8SScaled = 12,
    R8UInt = 13,
    R8SInt = 14,
    R8SRGB = 15,
    R8G8UNorm = 16,
    R8G8SNorm = 17,
    R8G8UScaled = 18,
    R8G8SScaled = 19,
    R8G8UInt = 20,
    R8G8SInt = 21,
    R8G8SRGB = 22,
    R8G8B8UNorm = 23,
    R8G8B8SNorm = 24,
    R8G8B8UScaled = 25,
    R8G8B8SScaled = 26,
    R8G8B8UInt = 27,
    R8G8B8SInt = 28,
    R8G8B8SRGB = 29,
    B8G8R8UNorm = 30,
    B8G8R8SNorm = 31,
    B8G8R8UScaled = 32,
    B8G8R8SScaled = 33,
    B8G8R8UInt = 34,
    B8G8R8SInt = 35,
    B8G8R8SRGB = 36,
    R8G8B8A8UNorm = 37,
    R8G8B8A8SNorm = 38,
    R8G8B8A8UScaled = 39,
    R8G8B8A8SScaled = 40,
    R8G8B8A8UInt = 41,
    R8G8B8A8SInt = 42,
    R8G8B8A8SRGB = 43,
    B8G8R8A8UNorm = 44,
    B8G8R8A8SNorm = 45,
    B8G8R8A8UScaled = 46,
    B8G8R8A8SScaled = 47,
    B8G8R8A8UInt = 48,
    B8G8R8A8SInt = 49,
    B8G8R8A8SRGB = 50,
    A8B8G8R8UNormPack32 = 51,
    A8B8G8R8SNormPack32 = 52,
    A8B8G8R8UScaledPack32 = 53,
    A8B8G8R8SScaledPack32 = 54,
    A8B8G8R8UIntPack32 = 55,
    A8B8G8R8SIntPack32 = 56,
    A8B8G8R8SRGBPack32 = 57,
    A2R10G10B10UNormPack32 = 58,
    A2R10G10B10SNormPack32 = 59,
    A2R10G10B10UScaledPack32 = 60,
    A2R10G10B10SScaledPack32 = 61,
    A2R10G10B10UIntPack32 = 62,
    A2R10G10B10SIntPack32 = 63,
    A2B10G10R10UNormPack32 = 64,
    A2B10G10R10SNormPack32 = 65,
    A2B10G10R10UScaledPack32 = 66,
    A2B10G10R10SScaledPack32 = 67,
    A2B10G10R10UIntPack32 = 68,
    A2B10G10R10SIntPack32 = 69,
    R16UNorm = 70,
    R16SNorm = 71,
    R16UScaled = 72,
    R16SScaled = 73,
    R16UInt = 74,
    R16SInt = 75,
    R16SFloat = 76,
    R16G16UNorm = 77,
    R16G16SNorm = 78,
    R16G16UScaled = 79,
    R16G16SScaled = 80,
    R16G16UInt = 81,
    R16G16SInt = 82,
    R16G16SFloat = 83,
    R16G16B16UNorm = 84,
    R16G16B16SNorm = 85,
    R16G16B16UScaled = 86,
    R16G16B16SScaled = 87,
    R16G16B16UInt = 88,
    R16G16B16SInt = 89,
    R16G16B16SFloat = 90,
    R16G16B16A16UNorm = 91,
    R16G16B16A16SNorm = 92,
    R16G16B16A16UScaled = 93,
    R16G16B16A16SScaled = 94,
    R16G16B16A16UInt = 95,
    R16G16B16A16SInt = 96,
    R16G16B16A16SFloat = 97,
    R32UInt = 98,
    R32SInt = 99,
    R32SFloat = 100,
    R32G32UInt = 101,
    R32G32SInt = 102,
    R32G32SFloat = 103,
    R32G32B32UInt = 104,
    R32G32B32SInt = 105,
    R32G32B32SFloat = 106,
    R32G32B32A32UInt = 107,
    R32G32B32A32SInt = 108,
    R32G32B32A32SFloat = 109,
    R64UInt = 110,
    R64SInt = 111,
    R64SFloat = 112,
    R64G64UInt = 113,
    R64G64SInt = 114,
    R64G64SFloat = 115,
    R64G64B64UInt = 116,
    R64G64B64SInt = 117,
    R64G64B64SFloat = 118,
    R64G64B64A64UInt = 119,
    R64G64B64A64SInt = 120,
    R64G64B64A64SFloat = 121,
    B10G11R11UFloatPack32 = 122,
    E5B9G9R9UFloatPack32 = 123,
    D16UNorm = 124,
    X8D24UNormPack32 = 125,
    D32SFloat = 126,
    S8UInt = 127,
    D16UNormS8UInt = 128,
    D24UNormS8UInt = 129,
    D32SFloatS8UInt = 130,
    Bc1RGBUNormBlock = 131,
    Bc1RGBSRGBBlock = 132,
    Bc1RGBAUNormBlock = 133,
    Bc1RGBASRGBBlock = 134,
    Bc2UNormBlock = 135,
    Bc2SRGBBlock = 136,
    Bc3UNormBlock = 137,
    Bc3SRGBBlock = 138,
    Bc4UNormBlock = 139,
    Bc4SNormBlock = 140,
    Bc5UNormBlock = 141,
    Bc5SNormBlock = 142,
    Bc6hUFloatBlock = 143,
    Bc6hSFloatBlock = 144,
    Bc7UNormBlock = 145,
    Bc7SRGBBlock = 146,
    Etc2R8G8B8UNormBlock = 147,
    Etc2R8G8B8SRGBBlock = 148,
    Etc2R8G8B8A1UNormBlock = 149,
    Etc2R8G8B8A1SRGBBlock = 150,
    Etc2R8G8B8A8UNormBlock = 151,
    Etc2R8G8B8A8SRGBBlock = 152,
    EacR11UNormBlock = 153,
    EacR11SNormBlock = 154,
    EacR11G11UNormBlock = 155,
    EacR11G11SNormBlock = 156,
    Astc4x4UNormBlock = 157,
    Astc4x4SRGBBlock = 158,
    Astc5x4UNormBlock = 159,
    Astc5x4SRGBBlock = 160,
    Astc5x5UNormBlock = 161,
    Astc5x5SRGBBlock = 162,
    Astc6x5UNormBlock = 163,
    Astc6x5SRGBBlock = 164,
    Astc6x6UNormBlock = 165,
    Astc6x6SRGBBlock = 166,
    Astc8x5UNormBlock = 167,
    Astc8x5SRGBBlock = 168,
    Astc8x6UNormBlock = 169,
    Astc8x6SRGBBlock = 170,
    Astc8x8UNormBlock = 171,
    Astc8x8SRGBBlock = 172,
    Astc10x5UNormBlock = 173,
    Astc10x5SRGBBlock = 174,
    Astc10x6UNormBlock = 175,
    Astc10x6SRGBBlock = 176,
    Astc10x8UNormBlock = 177,
    Astc10x8SRGBBlock = 178,
    Astc10x10UNormBlock = 179,
    Astc10x10SRGBBlock = 180,
    Astc12x10UNormBlock = 181,
    Astc12x10SRGBBlock = 182,
    Astc12x12UNormBlock = 183,
    Astc12x12SRGBBlock = 184,
    // Provided by VK_VERSION_1_1
    G8B8G8R8_422UNorm = 1000156000,
    B8G8R8G8_422UNorm = 1000156001,
    G8_B8_R8_3PLANE_420UNorm = 1000156002,
    G8_B8R8_2PLANE_420UNorm = 1000156003,
    G8_B8_R8_3PLANE_422UNorm = 1000156004,
    G8_B8R8_2PLANE_422UNorm = 1000156005,
    G8_B8_R8_3PLANE_444UNorm = 1000156006,
    R10X6UNormPack16 = 1000156007,
    R10X6G10X6UNorm_2PACK16 = 1000156008,
    R10X6G10X6B10X6A10X6UNorm_4PACK16 = 1000156009,
    G10X6B10X6G10X6R10X6_422UNorm_4PACK16 = 1000156010,
    B10X6G10X6R10X6G10X6_422UNorm_4PACK16 = 1000156011,
    G10X6_B10X6_R10X6_3PLANE_420UNorm_3PACK16 = 1000156012,
    G10X6_B10X6R10X6_2PLANE_420UNorm_3PACK16 = 1000156013,
    G10X6_B10X6_R10X6_3PLANE_422UNorm_3PACK16 = 1000156014,
    G10X6_B10X6R10X6_2PLANE_422UNorm_3PACK16 = 1000156015,
    G10X6_B10X6_R10X6_3PLANE_444UNorm_3PACK16 = 1000156016,
    R12X4UNormPack16 = 1000156017,
    R12X4G12X4UNorm_2PACK16 = 1000156018,
    R12X4G12X4B12X4A12X4UNorm_4PACK16 = 1000156019,
    G12X4B12X4G12X4R12X4_422UNorm_4PACK16 = 1000156020,
    B12X4G12X4R12X4G12X4_422UNorm_4PACK16 = 1000156021,
    G12X4_B12X4_R12X4_3PLANE_420UNorm_3PACK16 = 1000156022,
    G12X4_B12X4R12X4_2PLANE_420UNorm_3PACK16 = 1000156023,
    G12X4_B12X4_R12X4_3PLANE_422UNorm_3PACK16 = 1000156024,
    G12X4_B12X4R12X4_2PLANE_422UNorm_3PACK16 = 1000156025,
    G12X4_B12X4_R12X4_3PLANE_444UNorm_3PACK16 = 1000156026,
    G16B16G16R16_422UNorm = 1000156027,
    B16G16R16G16_422UNorm = 1000156028,
    G16_B16_R16_3PLANE_420UNorm = 1000156029,
    G16_B16R16_2PLANE_420UNorm = 1000156030,
    G16_B16_R16_3PLANE_422UNorm = 1000156031,
    G16_B16R16_2PLANE_422UNorm = 1000156032,
    G16_B16_R16_3PLANE_444UNorm = 1000156033,


    // Provided by VK_IMG_format_pvrtc
    PVRTC12BPP_UNormBlockIMG = 1000054000,
    PVRTC14BPP_UNormBlockIMG = 1000054001,
    PVRTC22BPP_UNormBlockIMG = 1000054002,
    PVRTC24BPP_UNormBlockIMG = 1000054003,
    PVRTC12BPP_SRGBBlockIMG = 1000054004,
    PVRTC14BPP_SRGBBlockIMG = 1000054005,
    PVRTC22BPP_SRGBBlockIMG = 1000054006,
    PVRTC24BPP_SRGBBlockIMG = 1000054007,

    // Provided by VK_EXT_texture_compression_astc_hdr
    Astc4x4SFloatBlockEXT = 1000066000,
    Astc5x4SFloatBlockEXT = 1000066001,
    Astc5x5SFloatBlockEXT = 1000066002,
    Astc6x5SFloatBlockEXT = 1000066003,
    Astc6x6SFloatBlockEXT = 1000066004,
    Astc8x5SFloatBlockEXT = 1000066005,
    Astc8x6SFloatBlockEXT = 1000066006,
    Astc8x8SFloatBlockEXT = 1000066007,
    Astc10x5SFloatBlockEXT = 1000066008,
    Astc10x6SFloatBlockEXT = 1000066009,
    Astc10x8SFloatBlockEXT = 1000066010,
    Astc10x10SFloatBlockEXT = 1000066011,
    Astc12x10SFloatBlockEXT = 1000066012,
    Astc12x12SFloatBlockEXT = 1000066013,

    // Provided by VK_EXT_4444_formats
    A4R4G4B4UNormPack16EXT = 1000340000,
    A4B4G4R4UNormPack16EXT = 1000340001,
  };

  /*struct R{
    u8 bits;

    friend constexpr bool operator ==(R a, R b) noexcept = default;
  };
  struct G{
    u8 bits;
  };
  struct B{
    u8 bits;
  };
  struct A{
    u8 bits;
  };
  template <typename T>
  struct Scaled;
  template <typename T>
  struct Normalized;

  template <typename T, auto ...Values>
  inline constexpr static Format format = Format::Undefined;
  template <>
  inline constexpr static Format format<Normalized<u8>, R{4}, G{4}> = Format::R4G4UNormPack8;*/





  using CullMode = Core::BitFlags<CullModeBits>;

  class VertexInputState{
    struct Info{
      u32 location;
      u32 offset;
      u32 stride;
      //Format
      bool vertexOrInstanceIndex;
    };
    Core::FlatMap<u32, Info> bindings;
  };
  class InputAssemblyState{
    struct Info{
      PrimitiveTopology topology;
      bool enableRestart;
    };
    Core::SmallArray<Info, 8> state;
  };
  class ViewportState{
    struct Info{
      float x;
      float y;
      float width;
      float height;
      Core::Interval<f32> depth;
      struct {
        u32 x;
        u32 y;
        u32 width;
        u32 height;
      } scissors;
    };
    Core::SmallArray<Info, 1> viewports;
  };
  class RasterizerState{
    bool enableDepthClamp;
    bool enableDiscard;
    PolygonMode polygonMode;
    CullMode cullMode;
    FrontFace frontFace;
    struct {
      bool enable;
      float constantFactor;
      float clamp;
      float slopeFactor;
    } depthBias;
    float lineWidth;
  };
  class MultisamplingState{
    SampleCount sampleCount; // REQUIRED
    u64 sampleMask; // OPTIONAL
    Core::Interval<f32, Core::UpperBound<0xFFFF'FFFF'FFFF'FFFFULL>> sampleShading; //OPTIONAL
    bool enableAlphaToCoverage; // OPTIONAL
    bool enableAlphaToOne;    // OPTIONAL
    bool enableSampleShading; // REQUIRED
  };
  class DepthStencilTestingState{
    struct {
      bool enableTest;
      bool enableWrite;
      bool enableBoundsTest;
      CompareOp compareOp;
      Core::Interval<float> bounds;
    } depth;
    struct {
      bool enableTest;
      struct {
        StencilOp failOp;
        StencilOp passOp;
        StencilOp depthFailOp;
        CompareOp compareOp;
        u32       compareMask;
        u32       writeMask;
        u32       reference;
      } front, back;
    } stencil;
  }; // OPTIONAL
  class ColorBlendingState{
    struct Attachment{
      bool enableBlend;
      struct {
        BlendOp blendOp;
        struct {
          BlendFactor src;
          BlendFactor dst;
        } blendFactor;
      } color, alpha;
      Core::BitFlags<ColorComponent> componentMask;
    };

    // Provided by VK_EXT_blend_operation_advanced
    BlendOverlap blendOverlap;
    bool srcPremultiplied;
    bool dstPremultiplied;

    // Basic
    bool enableLogicOp;
    LogicOp logicOp;
    struct {
      float r;
      float g;
      float b;
      float a;
    } blendConstant;
    Core::SmallArray<Attachment, 2> inputAttachmentStates;
  };
  class DynamicState{
    Core::SmallArray<DynamicStateFlags> dynamicStates;
  };

  struct DescriptorSetLayoutBinding{
    u32 binding;

  };

  struct PipelineLayout{

  };

  struct PipelineVertexInfo{
    VertexInputState input;
    InputAssemblyState inputAssembly;
    VertexShader shader;
  };

  struct FixedFunctions{
    VertexInputState vertexInput;
    InputAssemblyState inputAssembly;
    ViewportState viewport;
    RasterizerState rasterizer;
    MultisamplingState multisampling;
    DepthStencilTestingState depthStencilTesting;
    ColorBlendingState colorBlending;
    DynamicState dynamicState;
  };

  class ComputePipeline : public Pipeline {};
  class GraphicsPipeline : public Pipeline {
    std::optional<PipelineVertexInfo> vertex;
    ViewportState viewport;
    RasterizerState rasterizer;
    MultisamplingState multisampling;
    DepthStencilTestingState depthStencilTesting;
    ColorBlendingState colorBlending;
    DynamicState dynamicState;
  };
  class RayTracingPipeline : public Pipeline {};
  class PipelineLibrary : public VulkanType {};



  class DepthStencilPipeline : public GraphicsPipeline{

  };




  enum class PipelineStage {
    TopOfPipe = 0x1,
    DrawIndirect = 0x2,
    VertexInput = 0x4,
    VertexShader = 0x8,
    TessellationControlShader = 0x10,
    TessellationEvaluationShader = 0x20,
    GeometryShader = 0x40,
    FragmentShader = 0x80,
    EarlyFragmentTests = 0x100,
    LateFragmentTests = 0x200,
    ColorAttachmentOutput = 0x400,
    ComputeShader = 0x800,
    Transfer = 0x1000,
    BottomOfPipe = 0x2000,
    Host = 0x4000,
    AllGraphics = 0x8000,
    AllCommands = 0x10000,
    TransformFeedback = 0x0100'0000,
    ConditionalRendering = 0x0004'0000,
    CommandProcess = 0x0002'0000,
    RayTracingShader = 0x0020'0000,
    AccelerationStructureBuild = 0x0200'0000,
    TaskShader = 0x0008'0000,
    MeshShader = 0x0010'0000,
    FragmentDensityProcess = 0x0080'0000
  };
}

#endif//VALKYRIE_PIPELINE_HPP
