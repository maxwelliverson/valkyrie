//
// Created by Maxwell on 2020-12-02.
//

#ifndef VALKYRIE_VULKAN_ENUMS_HPP
#define VALKYRIE_VULKAN_ENUMS_HPP

#include <valkyrie/Core/Utility/BitFlags.hpp>

namespace valkyrie::Graphics::API {

  enum class StructureType : u32;
  enum class ObjectType : u32;

  enum class PrimitiveTopology {
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
  enum class PolygonMode {
    Fill,
    Line,
    Point,
    FillRectangleNV = 1000153000
  };
  enum class FrontFace {
    CounterClockwise,
    Clockwise
  };
  enum class CompareOp {
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always
  };
  enum class StencilOp {
    Keep,
    Zero,
    Replace,
    IncrementAndClamp,
    DecrementAndClamp,
    Invert,
    IncrementAndWrap,
    DecrementAndWrap
  };
  enum class BlendFactor {
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
  enum class BlendOp {
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
  enum class LogicOp {
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
  enum class BlendOverlap {
    Uncorrelated,
    Disjoint,
    Conjoint
  };
  enum class DynamicStateFlags {
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
  enum class DescriptorType {
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
  enum class Format {
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
  enum class SharingMode {
    Exclusive,
    Concurrent
  };
  enum class ColorSpace {
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
  enum class ComponentSwizzle {
    Identity = 0,
    Zero = 1,
    One = 2,
    R = 3,
    G = 4,
    B = 5,
    A = 6,
  };
  enum class PresentMode {
    Immediate = 0,
    Mailbox = 1,
    FIFO = 2,
    FIFORelaxed = 3,
    SharedDemandRefresh = 1000111000,
    SharedContinuousRefresh = 1000111001
  };


  namespace FlagBits {
    enum class CullMode {
      None = 0x0,
      Front = 0x1,
      Back = 0x2,
      FrontAndBack = 0x3
    };
    enum class ColorComponent {
      R = 0x1,
      G = 0x2,
      B = 0x4,
      A = 0x8
    };
    enum class SampleCount {
      x1 = 0x1,
      x2 = 0x2,
      x4 = 0x4,
      x8 = 0x8,
      x16 = 0x10,
      x32 = 0x20,
      x64 = 0x40
    };
    enum class SurfaceTransform {
      Identity = 0x1,
      Rotate90 = 0x2,
      Rotate180 = 0x4,
      Rotate270 = 0x8,
      HorizontalMirror = 0x10,
      HorizontalMirrorRotate90 = 0x20,
      HorizontalMirrorRotate180 = 0x40,
      HorizontalMirrorRotate270 = 0x80,
      Inherit = 0x100
    };
    enum class CompositeAlpha {
      Opaque = 0x1,
      PreMultiplied = 0x2,
      PostMultiplied = 0x4,
      Inherit = 0x8
    };
    enum class PresentMode {
      Local = 0x1,
      Remote = 0x2,
      Sum = 0x4,
      LocalMultiDevice = 0x8
    };

    enum class DescriptorSetLayout {
      UpdateAfterBindPool = 0x2,
      PushDescriptor = 0x1
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
    enum class Pipeline {
      DisableOptimization = 0x1,
      AllowDerivatives = 0x2,
      Derivative = 0x4,
      ViewIndexFromDeviceIndex = 0x8,
      DispatchBase = 0x10,
      RayTracingNoNullAnyHitShaders = 0x4000,
      RayTracingNoNullClosestHitShaders = 0x8000,
      RayTracingNoNullMissShaders = 0x10000,
      RayTracingNoNullIntersectionShaders = 0x20000,
      RayTracingSkipTriangles = 0x1000,
      RayTracingSkipAABBs = 0x2000,
      DeferCompile = 0x20,
      CaptureStatistics = 0x40,
      CaptureInternalRepresentations = 0x80,
      IndirectBindable = 0x40000,
      Library = 0x800,
      FailOnPipelineCompileRequired = 0x100,
      EarlyReturnOnFailure = 0x200
    };
    enum class ShaderStage {
      Vertex = 0x1,
      TessellationControl = 0x2,
      TessellationEvaluation = 0x4,
      Geometry = 0x8,
      Fragment = 0x10,
      Compute = 0x20,
      Task = 0x40,
      Mesh = 0x80,
      RayGen = 0x100,
      AnyHit = 0x200,
      ClosestHit = 0x400,
      Miss = 0x800,
      Intersection = 0x1000,
      Callable = 0x2000
    };


    enum class ImageUsage {
      TransferSrc = 0x1,
      TransferDst = 0x2,
      Sampled = 0x4,
      Storage = 0x8,
      ColorAttachment = 0x10,
      DepthStencilAttachment = 0x20,
      TransientAttachment = 0x40,
      InputAttachment = 0x80,
      ShadingRateImage = 0x100,
      FragmentDensityMap = 0x200
    };
    enum class ImageAspect {
      Color = 0x00000001,
      Depth = 0x00000002,
      Stencil = 0x00000004,
      Metadata = 0x00000008,

      // Provided by VK_VERSION_1_1
      Plane0 = 0x00000010,
      Plane1 = 0x00000020,
      Plane2 = 0x00000040,

      // Provided by VK_EXT_image_drm_format_modifier
      MemoryPlane0 = 0x00000080,
      MemoryPlane1 = 0x00000100,
      MemoryPlane2 = 0x00000200,
      MemoryPlane3 = 0x00000400
    };
    enum class BufferUsage {
      TransferBit = 0x1,
      TransferDst = 0x2,
      UniformTexelBuffer = 0x4,
      StorageTexelBuffer = 0x8,
      UniformBuffer = 0x10,
      StorageBuffer = 0x20,
      IndexBuffer = 0x40,
      VertexBuffer = 0x80,
      IndirectBuffer = 0x100,
      ShaderDeviceAddress = 0x20000,
      TransformFeedbackBuffer = 0x800,
      TransformFeedbackCounterBuffer = 0x1000,
      ConditionalRendering = 0x200,
      RayTracing = 0x400
    };
    enum class Queue {
      Graphics = 0x1,
      Compute = 0x2,
      Transfer = 0x4,
      SparseBinding = 0x8,
      Protected = 0x10
    };
    enum class PerformanceCounter {
      PerformanceImpacting = 0x00000001,
      ConcurrentlyImpacted = 0x00000002,
    };
    enum class CommandPool {
      Transient = 0x1,
      ResetCommandBuffer = 0x2,
      Protected = 0x4
    };
    enum class Swapchain {
      SplitInstanceBindRegions = 0x1,
      Protected = 0x2,
      MutableFormat = 0x4
    };

    enum class FormatFeature {
      SampledImage = 0x00000001,
      StorageImage = 0x00000002,
      StorageImageAtomic = 0x00000004,
      UniformTexelBuffer = 0x00000008,
      StorageTexelBuffer = 0x00000010,
      StorageTexelBufferAtomic = 0x00000020,
      VertexBuffer = 0x00000040,
      ColorAttachment = 0x00000080,
      ColorAttachmentBlend = 0x00000100,
      DepthStencilAttachment = 0x00000200,
      BlitSrc = 0x00000400,
      BlitDst = 0x00000800,
      SampledImageFilterLinear = 0x00001000,
      TransferSrc = 0x00004000,
      TransferDst = 0x00008000,
      MidpointChromaSamples = 0x00020000,
      SampledImageYcbcrConversionLinearFilter = 0x00040000,
      SampledImageYcbcrConversionSeparateReconstructionFilter = 0x00080000,
      SampledImageYcbcrConversionChromaReconstructionExplicit = 0x00100000,
      SampledImageYcbcrConversionChromaReconstructionExplicitForceable = 0x00200000,
      Disjoint = 0x00400000,
      CositedChromaSamples = 0x00800000,
      SampledImageFilterMinmax = 0x00010000,

      // Provided by VK_IMG_filter_cubic
      SampledImageFilterCubicIMG = 0x00002000,

      // Provided by VK_KHR_acceleration_structure
      AccelerationStructureVertexBufferKHR = 0x20000000,

      // Provided by VK_EXT_fragment_density_map
      FragmentDensityMapEXT = 0x01000000,

      // Provided by VK_KHR_fragment_shading_rate
      FragmentShadingRateAttachmentKHR = 0x40000000
    };

    enum class Access {
      IndirectCommandRead = 0x00000001,
      IndexRead = 0x00000002,
      VertexAttributeRead = 0x00000004,
      UniformRead = 0x00000008,
      InputAttachmentRead = 0x00000010,
      ShaderRead = 0x00000020,
      ShaderWrite = 0x00000040,
      ColorAttachmentRead = 0x00000080,
      ColorAttachmentWrite = 0x00000100,
      DepthStencilAttachmentRead = 0x00000200,
      DepthStencilAttachmentWrite = 0x00000400,
      TransferRead = 0x00000800,
      TransferWrite = 0x00001000,
      HostRead = 0x00002000,
      HostWrite = 0x00004000,
      MemoryRead = 0x00008000,
      MemoryWrite = 0x00010000,

      // Provided by VK_EXT_transform_feedback
      TransformFeedbackWriteEXT = 0x02000000,
      TransformFeedbackCounterReadEXT = 0x04000000,
      TransformFeedbackCounterWriteEXT = 0x08000000,

      // Provided by VK_EXT_conditional_rendering
      ConditionalRenderingReadEXT = 0x00100000,
      ColorAttachmentReadNoncoherentEXT = 0x00080000,

      // Provided by VK_KHR_acceleration_structure
      AccelerationStructureReadKHR = 0x00200000,
      AccelerationStructureWriteKHR = 0x00400000,

      // Provided by VK_NV_shading_rate_image
      ShadingRateImageReadNV = 0x00800000,

      // Provided by VK_EXT_fragment_density_map
      FragmentDensityMapReadEXT = 0x01000000,

      // Provided by VK_NV_device_generated_commands
      CommandPreprocessReadNV = 0x00020000,
      CommandPreprocessWriteNV = 0x00040000,

      // Provided by VK_NV_ray_tracing
      AccelerationStructureReadNV = AccelerationStructureReadKHR,
      AccelerationStructureWriteNV = AccelerationStructureWriteKHR,

      // Provided by VK_KHR_fragment_shading_rate
      FragmentShadingRateAttachmentRead = ShadingRateImageReadNV
    };


    // No aliases yet
    enum class Image {
      SparseBinding = 0x00000001,
      SparseResidency = 0x00000002,
      SparseAliased = 0x00000004,
      MutableFormat = 0x00000008,
      CubeCompatible = 0x00000010,
      Alias = 0x00000400,
      SplitInstanceBindRegions = 0x00000040,
      Array2DCompatible = 0x00000020,
      BlockTexelViewCompatible = 0x00000080,
      ExtendedUsage = 0x00000100,
      Protected = 0x00000800,
      Disjoint = 0x00000200,

      // Provided by VK_NV_corner_sampled_image
      CornerSampledNV = 0x00002000,

      // Provided by VK_EXT_sample_locations
      SampleLocationsCompatibleDepthEXT = 0x00001000,

      // Provided by VK_EXT_fragment_density_map
      SubsampledEXT = 0x00004000
    };
    enum class ImageView {

      // Provided by VK_EXT_fragment_density_map
      FragmentDensityMapDynamicEXT = 0x00000001,
      FragmentDensityMapDeferredEXT = 0x00000002,
    };
    enum class Buffer {
      SparseBinding = 0x00000001,
      SparseResidency = 0x00000002,
      SparseAliased = 0x00000004,
      Protected = 0x00000008,
      DeviceAddressCaptureReplay = 0x00000010,
    };
    enum class SparseImageFormat {
      SingleMiptail = 0x00000001,
      AlignedMipSize = 0x00000002,
      NonstandardBlockSize = 0x00000004,
    };

    enum class Framebuffer {
      Imageless = 0x1
    };

    enum class PipelineCreationFeedback {
      ValidEXT = 0x00000001,
      ApplicationPipelineCacheHitEXT = 0x00000002,
      BasePipelineAccelerationEXT = 0x00000004
    };
    enum class PipelineCache {
      ExternallySynchronizedEXT = 0x1
    };
    enum class PipelineShaderStage {
      AllowVaryingSubgroupSizeEXT = 0x00000001,
      RequireFullSubgroupsEXT = 0x00000002,
    };
    enum class Sampler {
      SubsampledEXT = 0x00000001,
      SubsampledCoarseReconstructionEXT = 0x00000002
    };
    enum class ResolveMode {
      SampleZero = 0x00000001,
      Average = 0x00000002,
      Min = 0x00000004,
      Max = 0x00000008
    };
    enum class DescriptorBinding {
      UpdateAfterBind = 0x00000001,
      UpdateUnusedWhilePending = 0x00000002,
      PartiallyBound = 0x00000004,
      VariableDescriptorCount = 0x00000008
    };
    enum class DescriptorPool {
      FreeDescriptorSet = 0x00000001,
      UpdateAfterBind = 0x00000002,
    };
    enum class AttachmentDescription {
      MayAlias = 0x1
    };
    enum class Dependency {
      ByRegion = 0x00000001,
      DeviceGroup = 0x00000004,
      ViewLocal = 0x00000002,
    };
    enum class RenderPass {
      TransformQCOM = 0x2
    };
    enum class SubpassDescription {
      PerViewAttributesNVX = 0x00000001,
      PerViewPositionXOnlyNVX = 0x00000002,
      FragmentRegionQCOM = 0x00000004,
      ShaderResolveQCOM = 0x00000008,
    };
    enum class CommandPoolReset {
      ReleaseResources = 0x1
    };
    enum class CommandBufferUsage {
      OneTimeSubmit = 0x00000001,
      RenderPassContinue = 0x00000002,
      SimultaneousUse = 0x00000004,
    };
    enum class CommandBufferReset {
      ReleaseResources = 0x1
    };
    enum class QueryPipelineStatistics {
      InputAssemblyVertices = 0x00000001,
      InputAssemblyPrimitives = 0x00000002,
      VertexShaderInvocations = 0x00000004,
      GeometryShaderInvocations = 0x00000008,
      GeometryShaderPrimitives = 0x00000010,
      ClippingInvocations = 0x00000020,
      ClippingPrimitives = 0x00000040,
      FragmentShaderInvocations = 0x00000080,
      TessellationControlShaderPatches = 0x00000100,
      TessellationEvaluationShaderInvocations = 0x00000200,
      ComputeShaderInvocations = 0x00000400,
    };
    enum class QueryResult {
      X64 = 0x00000001,
      Wait = 0x00000002,
      WithAvailability = 0x00000004,
      Partial = 0x00000008,
    };
    enum class QueryControl {
      Precise = 0x1
    };

    enum class IndirectState {
      FrontfaceNV = 0x00000001
    };
    enum class IndirectCommandsLayoutUsage {
      ExplicitPreprocessNV = 0x00000001,
      IndexedSequencesNV = 0x00000002,
      UnorderedSequencesNV = 0x00000004
    };

    enum class ConditionalRendering {
      InvertedEXT = 0x1
    };

    enum class Fence {
      Signaled = 0x1
    };

    enum class ExternalFenceFeature {
      Exportable = 0x1,
      Importable = 0x2
    };
    enum class ExternalFenceHandleType {
      OpaqueFd = 0x00000001,
      OpaqueWin32 = 0x00000002,
      OpaqueWin32Kmt = 0x00000004,
      SyncFd = 0x00000008
    };
    enum class FenceImport {
      Temporary = 0x1
    };

    enum class SemaphoreWait {
      Any = 0x1
    };

    enum class ExternalSemaphoreFeature {
      Exportable = 0x1,
      Importable = 0x2
    };
    enum class SemaphoreImport {
      Temporary = 0x1
    };
    enum class ExternalSemaphoreHandleType {
      OpaqueFd = 0x00000001,
      OpaqueWin32 = 0x00000002,
      OpaqueWin32Kmt = 0x00000004,
      D3D12Fence = 0x00000008,
      SyncFd = 0x00000010
    };

    enum class SparseMemoryBind {
      Metadata = 0x1
    };
    enum class MemoryHeap {
      DeviceLocal = 0x00000001,
      MultiInstance = 0x00000002,
    };
    enum class MemoryProperty {
      DeviceLocal = 0x00000001,
      HostVisible = 0x00000002,
      HostCoherent = 0x00000004,
      HostCached = 0x00000008,
      LazilyAllocated = 0x00000010,
      Protected = 0x00000020,
      DeviceCoherentAMD = 0x00000040,
      DeviceUncachedAMD = 0x00000080,
    };
    enum class PeerMemoryFeature {
      CopySrc = 0x00000001,
      CopyDst = 0x00000002,
      GenericSrc = 0x00000004,
      GenericDst = 0x00000008
    };
    enum class MemoryAllocate {
      DeviceMask = 0x00000001,
      DeviceAddress = 0x00000002,
      DeviceAddressCaptureReplay = 0x00000004
    };
    enum class ExternalMemoryHandleType {
      OpaqueFd = 0x00000001,
      OpaqueWin32 = 0x00000002,
      OpaqueWin32Kmt = 0x00000004,
      D3d11Texture = 0x00000008,
      D3d11TextureKmt = 0x00000010,
      D3d12Heap = 0x00000020,
      D3d12Resource = 0x00000040,
      DmaBufEXT = 0x00000200,
      AndroidHardwareBufferANDROID = 0x00000400,
      HostAllocationEXT = 0x00000080,
      HostMappedForeignMemoryEXT = 0x00000100
    };
    enum class ExternalMemoryFeature {
      DedicatedOnly = 0x00000001,
      Exportable = 0x00000002,
      Importable = 0x00000004,
    };

    enum class SubgroupFeature {
      Basic = 0x00000001,
      Vote = 0x00000002,
      Arithmetic = 0x00000004,
      Ballot = 0x00000008,
      Shuffle = 0x00000010,
      ShuffleRelative = 0x00000020,
      Clustered = 0x00000040,
      Quad = 0x00000080,
      PartitionedNV = 0x00000100,
    };

    enum class DebugMessageSeverity {
      Verbose = 0x00000001,
      Info = 0x00000010,
      Warning = 0x00000100,
      Error = 0x00001000
    };
    enum class DebugMessageType {
      General = 0x00000001,
      Validation = 0x00000002,
      Performance = 0x00000004
    };
    enum class ToolPurpose {
      Validation = 0x00000001,
      Profiling = 0x00000002,
      Tracing = 0x00000004,
      AdditionalFeatures = 0x00000008,
      ModifyingFeatures = 0x00000010,
      DebugReporting = 0x00000020,
      DebugMarkers = 0x00000040
    };
    enum class DeviceDiagnosticsConfig {
      EnableShaderDebugInfoNV = 0x00000001,
      EnableResourceTrackingNV = 0x00000002,
      EnableAutomaticCheckpointsNV = 0x00000004
    };

    enum class Geometry {
      Opaque = 0x00000001,
      NoDuplicateAnyHitInvocation = 0x00000002
    };
    enum class GeometryInstance {
      TriangleFacingCullDisable = 0x00000001,
      TriangleFrontCounterClockwise = 0x00000002,
      ForceOpaque = 0x00000004,
      ForceNoOpaque = 0x00000008
    };
    enum class BuildAccelerationStructure {
      AllowUpdate = 0x00000001,
      AllowCompaction = 0x00000002,
      PreferFastTrace = 0x00000004,
      PreferFastBuild = 0x00000008,
      LowMemory = 0x00000010
    };
  }
  namespace Flags {
    struct Pipeline;
    struct DescriptorSetLayout;
    struct ShaderStage;
    struct PipelineStage;
    struct CullMode;
    struct SampleCount;


    struct Swapchain;
    struct SurfaceTransform;
    struct CompositeAlpha;
    struct PresentMode;

    struct Access;

    struct ColorComponent;
    struct ImageUsage;
    struct BufferUsage;
    struct ImageAspect;
    struct FormatFeature;

    struct Queue;
    struct PerformanceCounter;
    struct CommandPool;
  }

  class CullModeFlags;

}

namespace valkyrie::Traits{
  template <>
  struct BitFlagEnum<Graphics::API::FlagBits::CullMode>{
    using bit_type = Graphics::API::FlagBits::CullMode;
    using flag_type = Graphics::API::CullModeFlags;
    inline constexpr static Core::StringView name{VK_string("CullMode")};
    inline constexpr static Core::StringView scoped_name{VK_string("valkyrie::Graphics::API::FlagBits::CullMode")};

  };
}




namespace valkyrie::Graphics::API{
  

#define VK_declare_flag_type(Type) class Type##Flags : public Core::CRTPBitFlags<Type##Flags, FlagBits::Type>{ \
    using Base_ = Core::CRTPBitFlags<Type##Flags, FlagBits::Type>;                                             \
  public:                                                                                                      \
    using Base_::Base_;                                                                                        \
    using Bit = Flags::Type;                                                                                   \
    constexpr Type##Flags() = default;                                                                         \
  }

#define PP_VK_impl_INLINE_FLAG_VALUE(Type, Flag) inline constexpr static Type##Flags Flag{ FlagBits::Type::Flag };
#define VK_inline_flags(Type, ...) VK_foreach(VK_bind_front(PP_VK_impl_INLINE_FLAG_VALUE, Type), __VA_ARGS__)
  
  
  VK_declare_flag_type(CullMode);
  VK_declare_flag_type(SampleCount);
  VK_declare_flag_type(ColorComponent);
  VK_declare_flag_type(ShaderStage);
  VK_declare_flag_type(PipelineStage);

  VK_declare_flag_type(SurfaceTransform);
  VK_declare_flag_type(CompositeAlpha);
  VK_declare_flag_type(PresentMode);
  
  VK_declare_flag_type(Access);
  
  VK_declare_flag_type(ImageUsage);
  VK_declare_flag_type(ImageAspect);
  VK_declare_flag_type(BufferUsage);
  VK_declare_flag_type(FormatFeature);
  

  VK_declare_flag_type(DescriptorSetLayout);
  VK_declare_flag_type(Pipeline);
  VK_declare_flag_type(Queue);
  VK_declare_flag_type(PerformanceCounter);
  VK_declare_flag_type(CommandPool);
  VK_declare_flag_type(Swapchain);
  
  namespace Flags{
    
    // = 0x[01248']+
    struct CullMode{
      CullMode() = delete;

      VK_inline_flags(CullMode,
                      None,
                      Front,
                      Back,
                      FrontAndBack)
    };
    struct SampleCount{
      VK_inline_flags(SampleCount,
                      x1,
                      x2,
                      x4,
                      x8,
                      x16,
                      x32,
                      x64)
    };
    struct ShaderStage{
      VK_inline_flags(ShaderStage,
                      Vertex,
                      TessellationControl,
                      TessellationEvaluation,
                      Geometry,
                      Fragment,
                      Compute,
                      Task,
                      Mesh,
                      RayGen,
                      AnyHit,
                      ClosestHit,
                      Miss,
                      Intersection,
                      Callable)
    };
    struct ColorComponent{
      VK_inline_flags(ColorComponent,
                      R,
                      G,
                      B,
                      A);
    };
    struct PipelineStage{
      VK_inline_flags(PipelineStage, 
                      TopOfPipe,
                      DrawIndirect,
                      VertexInput,
                      VertexShader,
                      TessellationControlShader,
                      TessellationEvaluationShader,
                      GeometryShader,
                      FragmentShader,
                      EarlyFragmentTests,
                      LateFragmentTests,
                      ColorAttachmentOutput,
                      ComputeShader,
                      Transfer,
                      BottomOfPipe,
                      Host,
                      AllGraphics,
                      AllCommands,
                      TransformFeedback,
                      ConditionalRendering,
                      CommandProcess,
                      RayTracingShader,
                      AccelerationStructureBuild,
                      TaskShader,
                      MeshShader,
                      FragmentDensityProcess);
    };
    
    
    struct SurfaceTransform{
      VK_inline_flags(SurfaceTransform,
                      Identity,
                      Rotate90,
                      Rotate180,
                      Rotate270,
                      HorizontalMirror,
                      HorizontalMirrorRotate90,
                      HorizontalMirrorRotate180,
                      HorizontalMirrorRotate270,
                      Inherit);
    };
    struct CompositeAlpha{
      VK_inline_flags(CompositeAlpha,
                      Opaque,
                      PreMultiplied,
                      PostMultiplied,
                      Inherit)
    };
    struct PresentMode{
      VK_inline_flags(PresentMode,
                      Local,
                      Remote,
                      Sum,
                      LocalMultiDevice)
    };
    
    struct Access{
      VK_inline_flags(Access,
                      IndirectCommandRead,
                      IndexRead,
                      VertexAttributeRead,
                      UniformRead,
                      InputAttachmentRead,
                      ShaderRead,
                      ShaderWrite,
                      ColorAttachmentRead,
                      ColorAttachmentWrite,
                      DepthStencilAttachmentRead,
                      DepthStencilAttachmentWrite,
                      TransferRead,
                      TransferWrite,
                      HostRead,
                      HostWrite,
                      MemoryRead,
                      MemoryWrite,
                      TransformFeedbackWriteEXT,
                      TransformFeedbackCounterReadEXT,
                      TransformFeedbackCounterWriteEXT,
                      ConditionalRenderingReadEXT,
                      ColorAttachmentReadNoncoherentEXT,
                      AccelerationStructureReadKHR,
                      AccelerationStructureWriteKHR,
                      ShadingRateImageReadNV,
                      FragmentDensityMapReadEXT,
                      CommandPreprocessReadNV,
                      CommandPreprocessWriteNV)
    };

    struct ImageAspect{
      VK_inline_flags(ImageAspect,
                      Color,
                      Depth,
                      Stencil,
                      Metadata,
                      Plane0,
                      Plane1,
                      Plane2,
                      MemoryPlane0,
                      MemoryPlane1,
                      MemoryPlane2,
                      MemoryPlane3)
    };
    struct ImageUsage{
      VK_inline_flags(ImageUsage,
                      TransferSrc,
                      TransferDst,
                      Sampled    ,
                      Storage    ,
                      ColorAttachment,
                      DepthStencilAttachment,
                      TransientAttachment,
                      InputAttachment,
                      ShadingRateImage,
                      FragmentDensityMap)
    };
    struct BufferUsage{
      VK_inline_flags(BufferUsage,
                      TransferBit,
                      TransferDst,
                      UniformTexelBuffer,
                      StorageTexelBuffer,
                      UniformBuffer,
                      StorageBuffer,
                      IndexBuffer,
                      VertexBuffer,
                      IndirectBuffer,
                      ShaderDeviceAddress,
                      TransformFeedbackBuffer,
                      TransformFeedbackCounterBuffer,
                      ConditionalRendering,
                      RayTracing)
    };
    struct FormatFeature{
      VK_inline_flags(FormatFeature,
                      SampledImage,
                      StorageImage,
                      StorageImageAtomic,
                      UniformTexelBuffer,
                      StorageTexelBuffer,
                      StorageTexelBufferAtomic,
                      VertexBuffer,
                      ColorAttachment,
                      ColorAttachmentBlend,
                      DepthStencilAttachment,
                      BlitSrc,
                      BlitDst,
                      SampledImageFilterLinear,
                      TransferSrc,
                      TransferDst,
                      MidpointChromaSamples,
                      SampledImageYcbcrConversionLinearFilter,
                      SampledImageYcbcrConversionSeparateReconstructionFilter,
                      SampledImageYcbcrConversionChromaReconstructionExplicit,
                      SampledImageYcbcrConversionChromaReconstructionExplicitForceable,
                      Disjoint,
                      CositedChromaSamples,
                      SampledImageFilterMinmax,
                      SampledImageFilterCubicIMG,
                      AccelerationStructureVertexBufferKHR,
                      FragmentDensityMapEXT,
                      FragmentShadingRateAttachmentKHR)
    };

    struct DescriptorSetLayout { 
      VK_inline_flags(DescriptorSetLayout,
                      UpdateAfterBindPool,
                      PushDescriptor) 
    };
    struct Pipeline { 
      VK_inline_flags(Pipeline,
                      DisableOptimization,
                      AllowDerivatives,
                      Derivative,
                      ViewIndexFromDeviceIndex,
                      DispatchBase,
                      RayTracingNoNullAnyHitShaders,
                      RayTracingNoNullClosestHitShaders,
                      RayTracingNoNullMissShaders,
                      RayTracingNoNullIntersectionShaders,
                      RayTracingSkipTriangles,
                      RayTracingSkipAABBs,
                      DeferCompile,
                      CaptureStatistics,
                      CaptureInternalRepresentations,
                      IndirectBindable,
                      Library,
                      FailOnPipelineCompileRequired,
                      EarlyReturnOnFailure) 
    };
    struct Queue { 
      VK_inline_flags(Queue,
                      Graphics,
                      Compute,
                      Transfer,
                      SparseBinding,
                      Protected) 
    };
    struct PerformanceCounter { 
      VK_inline_flags(PerformanceCounter,
                      PerformanceImpacting,
                      ConcurrentlyImpacted) 
    };
    struct CommandPool { 
      VK_inline_flags(CommandPool,
                      Transient,
                      ResetCommandBuffer,
                      Protected) 
    };
    struct Swapchain { 
      VK_inline_flags(Swapchain,
                      SplitInstanceBindRegions,
                      Protected,
                      MutableFormat) 
    };
  }

#undef VK_inline_flags
#undef VK_declare_flag_type
}

#endif//VALKYRIE_VULKAN_ENUMS_HPP
