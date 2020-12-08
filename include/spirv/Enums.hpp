//
// Created by maxwell on 2020-12-06.
//

#ifndef VALKYRIE_SPIRV_ENUMS_HPP
#define VALKYRIE_SPIRV_ENUMS_HPP

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Types.hpp>
#include <valkyrie/Core/Utility/BitFlags.hpp>

#define VK_inline_bitflag(flag_) inline constexpr static Core::BitFlags<bit_type> flag_ = bit_type::flag_

namespace valkyrie::Spv{
  namespace Bits {
    enum class ImageOperand {
      Bias = 0x1,
      Lod = 0x2,
      Grad = 0x4,
      ConstOffset = 0x8,
      Offset = 0x10,
      ConstOffsets = 0x20,
      Sample = 0x40,
      MinLod = 0x80,
      MakeTexelAvailable = 0x100,
      MakeTexelVisible = 0x200,
      NonPrivateTexel = 0x400,
      VolatileTexel = 0x800,
      SignExtend = 0x1000,
      ZeroExtend = 0x2000
    };
    enum class FPFastMathMode {
      NotNaN = 0x1,
      NotInf = 0x2,
      NSZ = 0x4,
      AllowRecip = 0x8,
      Fast = 0x10
    };
    enum class SelectionControl {
      Flatten = 0x1,
      DontFlatten = 0x2
    };
    enum class LoopControl {
      Unroll = 0x1,
      DontUnroll = 0x2,
      DependencyInfinite = 0x4,
      DependencyLength = 0x8,
      MinIterations = 0x10,
      MaxIterations = 0x20,
      IterationMultiple = 0x40,
      PeelCount = 0x80,
      PartialCount = 0x100,
      InitiationIntervalINTEL = 0x0001'0000,
      MaxConcurrencyINTEL = 0x0002'0000,
      DependencyArrayINTEL = 0x0004'0000,
      PipelineEnableINTEL = 0x0008'0000,
      LoopCoalesceINTEL = 0x0010'0000,
      MaxInterleavingINTEL = 0x0020'0000,
      SpeculatedIterationsINTEL = 0x0040'0000
    };
    enum class FunctionControl {
      Inline = 0x1,
      DontInline = 0x2,
      Pure = 0x4,
      Const = 0x8
    };
    enum class MemorySemantics {
      Acquire = 0x2,
      Release = 0x4,
      AcquireRelease = 0x8,
      SequentiallyConsistent = 0x10,
      UniformMemory = 0x40,
      SubgroupMemory = 0x80,
      WorkgroupMemory = 0x100,
      CrossWorkgroupMemory = 0x200,
      AtomicCounterMemory = 0x400,
      ImageMemory = 0x800,
      OutputMemory = 0x1000,
      MakeAvailable = 0x2000,
      MakeVisible = 0x4000,
      Volatile = 0x8000
    };
    enum class MemoryAccess {
      Volatile = 0x1,
      Aligned = 0x2,
      Nontemporal = 0x4,
      MakePointerAvailable = 0x8,
      MakePointerVisible = 0x10,
      NonPrivatePointer = 0x20
    };
    enum class KernelProfilingInfo {
      CmdExecTime = 0x1
    };
    enum class Ray{
      OpaqueKHR = 0x1,
      NoOpaqueKHR = 0x2,
      TerminateOnFirstHitKHR = 0x4,
      SkipClosestHitShaderKHR = 0x8,
      CullBackFacingTrianglesKHR = 0x10,
      CullFrontFacingTrianglesKHR = 0x20,
      CullOpaqueKHR = 0x40,
      CullNoOpaqueKHR = 0x80,
      SkipTrianglesKHR = 0x100,
      SkipAABBsKHR = 0x200
    };
    enum class ReflectType {
      Undefined = 0x00000000,
      Void = 0x00000001,
      Bool = 0x00000002,
      Int = 0x00000004,
      Float = 0x00000008,
      Vector = 0x00000100,
      Matrix = 0x00000200,
      ExternalImage = 0x00010000,
      ExternalSampler = 0x00020000,
      ExternalSampledImage = 0x00040000,
      ExternalBlock = 0x00080000,
      ExternalMask = 0x000F0000,
      Struct = 0x10000000,
      Array = 0x20000000,
    };
    enum class ReflectDecoration {
      None = 0x00000000,
      Block = 0x00000001,
      BufferBlock = 0x00000002,
      RowMajor = 0x00000004,
      ColumnMajor = 0x00000008,
      BuiltIn = 0x00000010,
      Noperspective = 0x00000020,
      Flat = 0x00000040,
      NonWritable = 0x00000080,
    };
    enum class ReflectShaderStage{
      Vertex                  = 0x00000001, // = VK_SHADER_STAGE_VERTEX_BIT
      TessellationControl    = 0x00000002, // = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
      TessellationEvaluation = 0x00000004, // = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
      Geometry            = 0x00000008, // = VK_SHADER_STAGE_GEOMETRY_BIT
      Fragment              = 0x00000010, // = VK_SHADER_STAGE_FRAGMENT_BIT
      Compute               = 0x00000020, // = VK_SHADER_STAGE_COMPUTE_BIT
    };
    enum class ReflectVariable{
      None = 0x0,
      Unused = 0x1
    };
  }

  enum class SourceLanguage {
    Unknown = 0,
    ESSL = 1,
    GLSL = 2,
    OpenCL_C = 3,
    OpenCL_CPP = 4,
    HLSL = 5
  };

  enum class ExecutionModel {
    Vertex = 0,
    TessellationControl = 1,
    TessellationEvaluation = 2,
    Geometry = 3,
    Fragment = 4,
    GLCompute = 5,
    Kernel = 6,
    TaskNV = 5267,
    MeshNV = 5268,
    RayGenerationKHR = 5313,
    RayGenerationNV = 5313,
    IntersectionKHR = 5314,
    IntersectionNV = 5314,
    AnyHitKHR = 5315,
    AnyHitNV = 5315,
    ClosestHitKHR = 5316,
    ClosestHitNV = 5316,
    MissKHR = 5317,
    MissNV = 5317,
    CallableKHR = 5318,
    CallableNV = 5318
  };

  enum class AddressingModel {
    Logical = 0,
    Physical32 = 1,
    Physical64 = 2,
    PhysicalStorageBuffer64 = 5348,
    PhysicalStorageBuffer64EXT = 5348
  };

  enum class MemoryModel {
    Simple = 0,
    GLSL450 = 1,
    OpenCL = 2,
    Vulkan = 3,
    VulkanKHR = 3
  };

  enum class ExecutionMode {
    Invocations = 0,
    SpacingEqual = 1,
    SpacingFractionalEven = 2,
    SpacingFractionalOdd = 3,
    VertexOrderCw = 4,
    VertexOrderCcw = 5,
    PixelCenterInteger = 6,
    OriginUpperLeft = 7,
    OriginLowerLeft = 8,
    EarlyFragmentTests = 9,
    PointMode = 10,
    Xfb = 11,
    DepthReplacing = 12,
    DepthGreater = 14,
    DepthLess = 15,
    DepthUnchanged = 16,
    LocalSize = 17,
    LocalSizeHint = 18,
    InputPoints = 19,
    InputLines = 20,
    InputLinesAdjacency = 21,
    Triangles = 22,
    InputTrianglesAdjacency = 23,
    Quads = 24,
    Isolines = 25,
    OutputVertices = 26,
    OutputPoints = 27,
    OutputLineStrip = 28,
    OutputTriangleStrip = 29,
    VecTypeHint = 30,
    ContractionOff = 31,
    Initializer = 33,
    Finalizer = 34,
    SubgroupSize = 35,
    SubgroupsPerWorkgroup = 36,
    SubgroupsPerWorkgroupId = 37,
    LocalSizeId = 38,
    LocalSizeHintId = 39,
    PostDepthCoverage = 4446,
    DenormPreserve = 4459,
    DenormFlushToZero = 4460,
    SignedZeroInfNanPreserve = 4461,
    RoundingModeRTE = 4462,
    RoundingModeRTZ = 4463,
    StencilRefReplacingEXT = 5027,
    OutputLinesNV = 5269,
    OutputPrimitivesNV = 5270,
    DerivativeGroupQuadsNV = 5289,
    DerivativeGroupLinearNV = 5290,
    OutputTrianglesNV = 5298,
    PixelInterlockOrderedEXT = 5366,
    PixelInterlockUnorderedEXT = 5367,
    SampleInterlockOrderedEXT = 5368,
    SampleInterlockUnorderedEXT = 5369,
    ShadingRateInterlockOrderedEXT = 5370,
    ShadingRateInterlockUnorderedEXT = 5371,
    MaxWorkgroupSizeINTEL = 5893,
    MaxWorkDimINTEL = 5894,
    NoGlobalOffsetINTEL = 5895,
    NumSIMDWorkitemsINTEL = 5896
  };

  enum class StorageClass {
    UniformConstant = 0,
    Input = 1,
    Uniform = 2,
    Output = 3,
    Workgroup = 4,
    CrossWorkgroup = 5,
    Private = 6,
    Function = 7,
    Generic = 8,
    PushConstant = 9,
    AtomicCounter = 10,
    Image = 11,
    StorageBuffer = 12,
    CallableDataKHR = 5328,
    CallableDataNV = 5328,
    IncomingCallableDataKHR = 5329,
    IncomingCallableDataNV = 5329,
    RayPayloadKHR = 5338,
    RayPayloadNV = 5338,
    HitAttributeKHR = 5339,
    HitAttributeNV = 5339,
    IncomingRayPayloadKHR = 5342,
    IncomingRayPayloadNV = 5342,
    ShaderRecordBufferKHR = 5343,
    ShaderRecordBufferNV = 5343,
    PhysicalStorageBuffer = 5349,
    PhysicalStorageBufferEXT = 5349,
    CodeSectionINTEL = 5605
  };

  enum class Dim {
    D1 = 0,
    D2 = 1,
    D3 = 2,
    Cube = 3,
    Rect = 4,
    Buffer = 5,
    SubpassData = 6
  };

  enum class SamplerAddressingMode {
    None = 0,
    ClampToEdge = 1,
    Clamp = 2,
    Repeat = 3,
    RepeatMirrored = 4
  };

  enum class SamplerFilterMode {
    Nearest = 0,
    Linear = 1
  };

  enum class ImageFormat {
    Unknown = 0,
    Rgba32f = 1,
    Rgba16f = 2,
    R32f = 3,
    Rgba8 = 4,
    Rgba8Snorm = 5,
    Rg32f = 6,
    Rg16f = 7,
    R11fG11fB10f = 8,
    R16f = 9,
    Rgba16 = 10,
    Rgb10A2 = 11,
    Rg16 = 12,
    Rg8 = 13,
    R16 = 14,
    R8 = 15,
    Rgba16Snorm = 16,
    Rg16Snorm = 17,
    Rg8Snorm = 18,
    R16Snorm = 19,
    R8Snorm = 20,
    Rgba32i = 21,
    Rgba16i = 22,
    Rgba8i = 23,
    R32i = 24,
    Rg32i = 25,
    Rg16i = 26,
    Rg8i = 27,
    R16i = 28,
    R8i = 29,
    Rgba32ui = 30,
    Rgba16ui = 31,
    Rgba8ui = 32,
    R32ui = 33,
    Rgb10a2ui = 34,
    Rg32ui = 35,
    Rg16ui = 36,
    Rg8ui = 37,
    R16ui = 38,
    R8ui = 39,
  };

  enum class ImageChannelOrder {
    R = 0,
    A = 1,
    RG = 2,
    RA = 3,
    RGB = 4,
    RGBA = 5,
    BGRA = 6,
    ARGB = 7,
    Intensity = 8,
    Luminance = 9,
    Rx = 10,
    RGx = 11,
    RGBx = 12,
    Depth = 13,
    DepthStencil = 14,
    sRGB = 15,
    sRGBx = 16,
    sRGBA = 17,
    sBGRA = 18,
    ABGR = 19
  };

  enum class ImageChannelDataType {
    SnormInt8 = 0,
    SnormInt16 = 1,
    UnormInt8 = 2,
    UnormInt16 = 3,
    UnormShort565 = 4,
    UnormShort555 = 5,
    UnormInt101010 = 6,
    SignedInt8 = 7,
    SignedInt16 = 8,
    SignedInt32 = 9,
    UnsignedInt8 = 10,
    UnsignedInt16 = 11,
    UnsignedInt32 = 12,
    HalfFloat = 13,
    Float = 14,
    UnormInt24 = 15,
    UnormInt101010_2 = 16,
  };

  class ImageOperand : public Core::BitFlags<Bits::ImageOperand> {
  public:
    using enum_type = ImageOperand;
    using bit_type = Bits::ImageOperand;

    using Core::BitFlags<Bits::ImageOperand>::BitFlags;

    VK_inline_bitflag(Bias);
    VK_inline_bitflag(Lod);
    VK_inline_bitflag(Grad);
    VK_inline_bitflag(ConstOffset);
    VK_inline_bitflag(Offset);
    VK_inline_bitflag(ConstOffsets);
    VK_inline_bitflag(Sample);
    VK_inline_bitflag(MinLod);
    VK_inline_bitflag(MakeTexelAvailable);
    VK_inline_bitflag(MakeTexelVisible);
    VK_inline_bitflag(NonPrivateTexel);
    VK_inline_bitflag(VolatileTexel);
    VK_inline_bitflag(SignExtend);
    VK_inline_bitflag(ZeroExtend);
  };

  class FPFastMathMode : public Core::BitFlags<Bits::FPFastMathMode> {
  public:
    using enum_type = FPFastMathMode;
    using bit_type = Bits::FPFastMathMode;
    using underlying_type = std::underlying_type_t<bit_type>;

    using Core::BitFlags<Bits::FPFastMathMode>::BitFlags;

    VK_inline_bitflag(NotNaN);
    VK_inline_bitflag(NotInf);
    VK_inline_bitflag(NSZ);
    VK_inline_bitflag(AllowRecip);
    VK_inline_bitflag(Fast);
  };

  enum class FPRoundingMode {
    RTE = 0,
    RTZ = 1,
    RTP = 2,
    RTN = 3
  };

  enum class LinkageType {
    Export = 0,
    Import = 1
  };

  class SelectionControl : public Core::BitFlags<Bits::SelectionControl> {
  public:
    using enum_type = SelectionControl;
    using bit_type = Bits::SelectionControl;

    using Core::BitFlags<Bits::SelectionControl>::BitFlags;

    VK_inline_bitflag(Flatten);
    VK_inline_bitflag(DontFlatten);
  };

  class LoopControl : public Core::BitFlags<Bits::LoopControl> {
  public:
    using enum_type = LoopControl;
    using bit_type = Bits::LoopControl;

    using Core::BitFlags<Bits::LoopControl>::BitFlags;

    VK_inline_bitflag(Unroll);
    VK_inline_bitflag(DontUnroll);
    VK_inline_bitflag(DependencyInfinite);
    VK_inline_bitflag(DependencyLength);
    VK_inline_bitflag(MinIterations);
    VK_inline_bitflag(MaxIterations);
    VK_inline_bitflag(IterationMultiple);
    VK_inline_bitflag(PeelCount);
    VK_inline_bitflag(PartialCount);
    VK_inline_bitflag(InitiationIntervalINTEL);
    VK_inline_bitflag(MaxConcurrencyINTEL);
    VK_inline_bitflag(DependencyArrayINTEL);
    VK_inline_bitflag(PipelineEnableINTEL);
    VK_inline_bitflag(LoopCoalesceINTEL);
    VK_inline_bitflag(MaxInterleavingINTEL);
    VK_inline_bitflag(SpeculatedIterationsINTEL);
  };

  class FunctionControl : public Core::BitFlags<Bits::FunctionControl> {
  public:
    using enum_type = FunctionControl;
    using bit_type = Bits::FunctionControl;

    using Core::BitFlags<Bits::FunctionControl>::BitFlags;

    VK_inline_bitflag(Inline);
    VK_inline_bitflag(DontInline);
    VK_inline_bitflag(Pure);
    VK_inline_bitflag(Const);
  };

  class MemorySemantics : public Core::BitFlags<Bits::MemorySemantics> {
  public:
    using enum_type = MemorySemantics;
    using bit_type = Bits::MemorySemantics;

    using Core::BitFlags<Bits::MemorySemantics>::BitFlags;

    VK_inline_bitflag(Acquire);
    VK_inline_bitflag(Release);
    VK_inline_bitflag(AcquireRelease);
    VK_inline_bitflag(SequentiallyConsistent);
    VK_inline_bitflag(UniformMemory);
    VK_inline_bitflag(SubgroupMemory);
    VK_inline_bitflag(WorkgroupMemory);
    VK_inline_bitflag(CrossWorkgroupMemory);
    VK_inline_bitflag(AtomicCounterMemory);
    VK_inline_bitflag(ImageMemory);
    VK_inline_bitflag(OutputMemory);
    VK_inline_bitflag(MakeAvailable);
    VK_inline_bitflag(MakeVisible);
    VK_inline_bitflag(Volatile);
  };

  class MemoryAccess : public Core::BitFlags<Bits::MemoryAccess> {
  public:
    using enum_type = MemoryAccess;
    using bit_type = Bits::MemoryAccess;

    using Core::BitFlags<Bits::MemoryAccess>::BitFlags;

    VK_inline_bitflag(Volatile);
    VK_inline_bitflag(Aligned);
    VK_inline_bitflag(Nontemporal);
    VK_inline_bitflag(MakePointerAvailable);
    VK_inline_bitflag(MakePointerVisible);
    VK_inline_bitflag(NonPrivatePointer);
  };

  enum class AccessQualifier {
    ReadOnly = 0,
    WriteOnly = 1,
    ReadWrite = 2
  };

  enum class FunctionParameterAttribute {
    Zext = 0,
    Sext = 1,
    ByVal = 2,
    Sret = 3,
    NoAlias = 4,
    NoCapture = 5,
    NoWrite = 6,
    NoReadWrite = 7
  };

  enum class Decoration {
    RelaxedPrecision = 0,
    SpecId = 1,
    Block = 2,
    BufferBlock = 3,
    RowMajor = 4,
    ColMajor = 5,
    ArrayStride = 6,
    MatrixStride = 7,
    GLSLShared = 8,
    GLSLPacked = 9,
    CPacked = 10,
    BuiltIn = 11,
    NoPerspective = 13,
    Flat = 14,
    Patch = 15,
    Centroid = 16,
    Sample = 17,
    Invariant = 18,
    Restrict = 19,
    Aliased = 20,
    Volatile = 21,
    Constant = 22,
    Coherent = 23,
    NonWritable = 24,
    NonReadable = 25,
    Uniform = 26,
    UniformId = 27,
    SaturatedConversion = 28,
    Stream = 29,
    Location = 30,
    Component = 31,
    Index = 32,
    Binding = 33,
    DescriptorSet = 34,
    Offset = 35,
    XfbBuffer = 36,
    XfbStride = 37,
    FuncParamAttr = 38,
    FPRoundingMode = 39,
    FPFastMathMode = 40,
    LinkageAttributes = 41,
    NoContraction = 42,
    InputAttachmentIndex = 43,
    Alignment = 44,
    MaxByteOffset = 45,
    AlignmentId = 46,
    MaxByteOffsetId = 47,
    NoSignedWrap = 4469,
    NoUnsignedWrap = 4470,
    ExplicitInterpAMD = 4999,
    OverrideCoverageNV = 5248,
    PassthroughNV = 5250,
    ViewportRelativeNV = 5252,
    SecondaryViewportRelativeNV = 5256,
    PerPrimitiveNV = 5271,
    PerViewNV = 5272,
    PerTaskNV = 5273,
    PerVertexNV = 5285,
    NonUniform = 5300,
    NonUniformEXT = 5300,
    RestrictPointer = 5355,
    RestrictPointerEXT = 5355,
    AliasedPointer = 5356,
    AliasedPointerEXT = 5356,
    ReferencedIndirectlyINTEL = 5602,
    CounterBuffer = 5634,
    HlslCounterBufferGOOGLE = 5634,
    HlslSemanticGOOGLE = 5635,
    UserSemantic = 5635,
    UserTypeGOOGLE = 5636,
    RegisterINTEL = 5825,
    MemoryINTEL = 5826,
    NumbanksINTEL = 5827,
    BankwidthINTEL = 5828,
    MaxPrivateCopiesINTEL = 5829,
    SinglepumpINTEL = 5830,
    DoublepumpINTEL = 5831,
    MaxReplicatesINTEL = 5832,
    SimpleDualPortINTEL = 5833,
    MergeINTEL = 5834,
    BankBitsINTEL = 5835,
    ForcePow2DepthINTEL = 5836
  };

  enum class BuiltIn {
    Position = 0,
    PointSize = 1,
    ClipDistance = 3,
    CullDistance = 4,
    VertexId = 5,
    InstanceId = 6,
    PrimitiveId = 7,
    InvocationId = 8,
    Layer = 9,
    ViewportIndex = 10,
    TessLevelOuter = 11,
    TessLevelInner = 12,
    TessCoord = 13,
    PatchVertices = 14,
    FragCoord = 15,
    PointCoord = 16,
    FrontFacing = 17,
    SampleId = 18,
    SamplePosition = 19,
    SampleMask = 20,
    FragDepth = 22,
    HelperInvocation = 23,
    NumWorkgroups = 24,
    WorkgroupSize = 25,
    WorkgroupId = 26,
    LocalInvocationId = 27,
    GlobalInvocationId = 28,
    LocalInvocationIndex = 29,
    WorkDim = 30,
    GlobalSize = 31,
    EnqueuedWorkgroupSize = 32,
    GlobalOffset = 33,
    GlobalLinearId = 34,
    SubgroupSize = 36,
    SubgroupMaxSize = 37,
    NumSubgroups = 38,
    NumEnqueuedSubgroups = 39,
    SubgroupId = 40,
    SubgroupLocalInvocationId = 41,
    VertexIndex = 42,
    InstanceIndex = 43,
    SubgroupEqMask = 4416,
    SubgroupEqMaskKHR = 4416,
    SubgroupGeMask = 4417,
    SubgroupGeMaskKHR = 4417,
    SubgroupGtMask = 4418,
    SubgroupGtMaskKHR = 4418,
    SubgroupLeMask = 4419,
    SubgroupLeMaskKHR = 4419,
    SubgroupLtMask = 4420,
    SubgroupLtMaskKHR = 4420,
    BaseVertex = 4424,
    BaseInstance = 4425,
    DrawIndex = 4426,
    DeviceIndex = 4438,
    ViewIndex = 4440,
    BaryCoordNoPerspAMD = 4992,
    BaryCoordNoPerspCentroidAMD = 4993,
    BaryCoordNoPerspSampleAMD = 4994,
    BaryCoordSmoothAMD = 4995,
    BaryCoordSmoothCentroidAMD = 4996,
    BaryCoordSmoothSampleAMD = 4997,
    BaryCoordPullModelAMD = 4998,
    FragStencilRefEXT = 5014,
    ViewportMaskNV = 5253,
    SecondaryPositionNV = 5257,
    SecondaryViewportMaskNV = 5258,
    PositionPerViewNV = 5261,
    ViewportMaskPerViewNV = 5262,
    FullyCoveredEXT = 5264,
    TaskCountNV = 5274,
    PrimitiveCountNV = 5275,
    PrimitiveIndicesNV = 5276,
    ClipDistancePerViewNV = 5277,
    CullDistancePerViewNV = 5278,
    LayerPerViewNV = 5279,
    MeshViewCountNV = 5280,
    MeshViewIndicesNV = 5281,
    BaryCoordNV = 5286,
    BaryCoordNoPerspNV = 5287,
    FragSizeEXT = 5292,
    FragmentSizeNV = 5292,
    FragInvocationCountEXT = 5293,
    InvocationsPerPixelNV = 5293,
    LaunchIdKHR = 5319,
    LaunchIdNV = 5319,
    LaunchSizeKHR = 5320,
    LaunchSizeNV = 5320,
    WorldRayOriginKHR = 5321,
    WorldRayOriginNV = 5321,
    WorldRayDirectionKHR = 5322,
    WorldRayDirectionNV = 5322,
    ObjectRayOriginKHR = 5323,
    ObjectRayOriginNV = 5323,
    ObjectRayDirectionKHR = 5324,
    ObjectRayDirectionNV = 5324,
    RayTminKHR = 5325,
    RayTminNV = 5325,
    RayTmaxKHR = 5326,
    RayTmaxNV = 5326,
    InstanceCustomIndexKHR = 5327,
    InstanceCustomIndexNV = 5327,
    ObjectToWorldKHR = 5330,
    ObjectToWorldNV = 5330,
    WorldToObjectKHR = 5331,
    WorldToObjectNV = 5331,
    HitTKHR = 5332,
    HitTNV = 5332,
    HitKindKHR = 5333,
    HitKindNV = 5333,
    IncomingRayFlagsKHR = 5351,
    IncomingRayFlagsNV = 5351,
    RayGeometryIndexKHR = 5352,
    WarpsPerSMNV = 5374,
    SMCountNV = 5375,
    WarpIDNV = 5376,
    SMIDNV = 5377
  };

  enum class Scope {
    CrossDevice = 0,
    Device = 1,
    Workgroup = 2,
    Subgroup = 3,
    Invocation = 4,
    QueueFamily = 5,
    ShaderCallKHR = 6
  };

  enum class GroupOperation {
    Reduce = 0,
    InclusiveScan = 1,
    ExclusiveScan = 2,
    ClusteredReduce = 3,
    PartitionedReduceNV = 6,
    PartitionedInclusiveScanNV = 7,
    PartitionedExclusiveScanNV = 8
  };

  enum class KernelEnqueueFlags {
    NoWait = 0,
    WaitKernel = 1,
    WaitWorkGroup = 2
  };

  class KernelProfilingInfo : public Core::BitFlags<Bits::KernelProfilingInfo> {
  public:
    using enum_type = KernelProfilingInfo;
    using bit_type = Bits::KernelProfilingInfo;

    using Core::BitFlags<Bits::KernelProfilingInfo>::BitFlags;

    VK_inline_bitflag(CmdExecTime);
  };

  enum class Capability {
    Matrix = 0,
    Shader = 1,
    Geometry = 2,
    Tessellation = 3,
    Addresses = 4,
    Linkage = 5,
    Kernel = 6,
    Vector16 = 7,
    Float16Buffer = 8,
    Float16 = 9,
    Float64 = 10,
    Int64 = 11,
    Int64Atomics = 12,
    ImageBasic = 13,
    ImageReadWrite = 14,
    ImageMipmap = 15,
    Pipes = 17,
    Groups = 18,
    DeviceEnqueue = 19,
    LiteralSampler = 20,
    AtomicStorage = 21,
    Int16 = 22,
    TessellationPointSize = 23,
    GeometryPointSize = 24,
    ImageGatherExtended = 25,
    StorageImageMultisample = 27,
    UniformBufferArrayDynamicIndexing = 28,
    SampledImageArrayDynamicIndexing = 29,
    StorageBufferArrayDynamicIndexing = 30,
    StorageImageArrayDynamicIndexing = 31,
    ClipDistance = 32,
    CullDistance = 33,
    ImageCubeArray = 34,
    SampleRateShading = 35,
    ImageRect = 36,
    SampledRect = 37,
    GenericPointer = 38,
    Int8 = 39,
    InputAttachment = 40,
    SparseResidency = 41,
    MinLod = 42,
    Sampled1D = 43,
    Image1D = 44,
    SampledCubeArray = 45,
    SampledBuffer = 46,
    ImageBuffer = 47,
    ImageMSArray = 48,
    StorageImageExtendedFormats = 49,
    ImageQuery = 50,
    DerivativeControl = 51,
    InterpolationFunction = 52,
    TransformFeedback = 53,
    GeometryStreams = 54,
    StorageImageReadWithoutFormat = 55,
    StorageImageWriteWithoutFormat = 56,
    MultiViewport = 57,
    SubgroupDispatch = 58,
    NamedBarrier = 59,
    PipeStorage = 60,
    GroupNonUniform = 61,
    GroupNonUniformVote = 62,
    GroupNonUniformArithmetic = 63,
    GroupNonUniformBallot = 64,
    GroupNonUniformShuffle = 65,
    GroupNonUniformShuffleRelative = 66,
    GroupNonUniformClustered = 67,
    GroupNonUniformQuad = 68,
    ShaderLayer = 69,
    ShaderViewportIndex = 70,
    SubgroupBallotKHR = 4423,
    DrawParameters = 4427,
    SubgroupVoteKHR = 4431,
    StorageBuffer16BitAccess = 4433,
    StorageUniformBufferBlock16 = 4433,
    StorageUniform16 = 4434,
    UniformAndStorageBuffer16BitAccess = 4434,
    StoragePushConstant16 = 4435,
    StorageInputOutput16 = 4436,
    DeviceGroup = 4437,
    MultiView = 4439,
    VariablePointersStorageBuffer = 4441,
    VariablePointers = 4442,
    AtomicStorageOps = 4445,
    SampleMaskPostDepthCoverage = 4447,
    StorageBuffer8BitAccess = 4448,
    UniformAndStorageBuffer8BitAccess = 4449,
    StoragePushConstant8 = 4450,
    DenormPreserve = 4464,
    DenormFlushToZero = 4465,
    SignedZeroInfNanPreserve = 4466,
    RoundingModeRTE = 4467,
    RoundingModeRTZ = 4468,
    RayQueryProvisionalKHR = 4471,
    RayTraversalPrimitiveCullingProvisionalKHR = 4478,
    Float16ImageAMD = 5008,
    ImageGatherBiasLodAMD = 5009,
    FragmentMaskAMD = 5010,
    StencilExportEXT = 5013,
    ImageReadWriteLodAMD = 5015,
    ShaderClockKHR = 5055,
    SampleMaskOverrideCoverageNV = 5249,
    GeometryShaderPassthroughNV = 5251,
    ShaderViewportIndexLayerEXT = 5254,
    ShaderViewportIndexLayerNV = 5254,
    ShaderViewportMaskNV = 5255,
    ShaderStereoViewNV = 5259,
    PerViewAttributesNV = 5260,
    FragmentFullyCoveredEXT = 5265,
    MeshShadingNV = 5266,
    ImageFootprintNV = 5282,
    FragmentBarycentricNV = 5284,
    ComputeDerivativeGroupQuadsNV = 5288,
    FragmentDensityEXT = 5291,
    ShadingRateNV = 5291,
    GroupNonUniformPartitionedNV = 5297,
    ShaderNonUniform = 5301,
    ShaderNonUniformEXT = 5301,
    RuntimeDescriptorArray = 5302,
    RuntimeDescriptorArrayEXT = 5302,
    InputAttachmentArrayDynamicIndexing = 5303,
    InputAttachmentArrayDynamicIndexingEXT = 5303,
    UniformTexelBufferArrayDynamicIndexing = 5304,
    UniformTexelBufferArrayDynamicIndexingEXT = 5304,
    StorageTexelBufferArrayDynamicIndexing = 5305,
    StorageTexelBufferArrayDynamicIndexingEXT = 5305,
    UniformBufferArrayNonUniformIndexing = 5306,
    UniformBufferArrayNonUniformIndexingEXT = 5306,
    SampledImageArrayNonUniformIndexing = 5307,
    SampledImageArrayNonUniformIndexingEXT = 5307,
    StorageBufferArrayNonUniformIndexing = 5308,
    StorageBufferArrayNonUniformIndexingEXT = 5308,
    StorageImageArrayNonUniformIndexing = 5309,
    StorageImageArrayNonUniformIndexingEXT = 5309,
    InputAttachmentArrayNonUniformIndexing = 5310,
    InputAttachmentArrayNonUniformIndexingEXT = 5310,
    UniformTexelBufferArrayNonUniformIndexing = 5311,
    UniformTexelBufferArrayNonUniformIndexingEXT = 5311,
    StorageTexelBufferArrayNonUniformIndexing = 5312,
    StorageTexelBufferArrayNonUniformIndexingEXT = 5312,
    RayTracingNV = 5340,
    VulkanMemoryModel = 5345,
    VulkanMemoryModelKHR = 5345,
    VulkanMemoryModelDeviceScope = 5346,
    VulkanMemoryModelDeviceScopeKHR = 5346,
    PhysicalStorageBufferAddresses = 5347,
    PhysicalStorageBufferAddressesEXT = 5347,
    ComputeDerivativeGroupLinearNV = 5350,
    RayTracingProvisionalKHR = 5353,
    CooperativeMatrixNV = 5357,
    FragmentShaderSampleInterlockEXT = 5363,
    FragmentShaderShadingRateInterlockEXT = 5372,
    ShaderSMBuiltinsNV = 5373,
    FragmentShaderPixelInterlockEXT = 5378,
    DemoteToHelperInvocationEXT = 5379,
    SubgroupShuffleINTEL = 5568,
    SubgroupBufferBlockIOINTEL = 5569,
    SubgroupImageBlockIOINTEL = 5570,
    SubgroupImageMediaBlockIOINTEL = 5579,
    IntegerFunctions2INTEL = 5584,
    FunctionPointersINTEL = 5603,
    IndirectReferencesINTEL = 5604,
    SubgroupAvcMotionEstimationINTEL = 5696,
    SubgroupAvcMotionEstimationIntraINTEL = 5697,
    SubgroupAvcMotionEstimationChromaINTEL = 5698,
    FPGAMemoryAttributesINTEL = 5824,
    UnstructuredLoopControlsINTEL = 5886,
    FPGALoopControlsINTEL = 5888,
    KernelAttributesINTEL = 5892,
    FPGAKernelAttributesINTEL = 5897,
    BlockingPipesINTEL = 5945,
    FPGARegINTEL = 5948,
    AtomicFloat32AddEXT = 6033,
    AtomicFloat64AddEXT = 6034
  };

  class Ray : public Core::BitFlags<Bits::Ray>{
  public:
    using enum_type = Ray;
    using bit_type  = Bits::Ray;

    using Core::BitFlags<Bits::Ray>::BitFlags;

    VK_inline_bitflag(OpaqueKHR);
    VK_inline_bitflag(NoOpaqueKHR);
    VK_inline_bitflag(TerminateOnFirstHitKHR);
    VK_inline_bitflag(SkipClosestHitShaderKHR);
    VK_inline_bitflag(CullBackFacingTrianglesKHR);
    VK_inline_bitflag(CullFrontFacingTrianglesKHR);
    VK_inline_bitflag(CullOpaqueKHR);
    VK_inline_bitflag(CullNoOpaqueKHR);
    VK_inline_bitflag(SkipTrianglesKHR);
    VK_inline_bitflag(SkipAABBsKHR);
  };

  enum class RayQueryIntersection : unsigned {
    RayQueryCandidateIntersectionKHR = 0,
    RayQueryCommittedIntersectionKHR = 1
  };

  enum class RayQueryCommittedIntersectionType : unsigned {
    RayQueryCommittedIntersectionNoneKHR = 0,
    RayQueryCommittedIntersectionTriangleKHR = 1,
    RayQueryCommittedIntersectionGeneratedKHR = 2
  };

  enum class RayQueryCandidateIntersectionType : unsigned {
    RayQueryCandidateIntersectionTriangleKHR = 0,
    RayQueryCandidateIntersectionAABBKHR = 1
  };

  enum class Op {
    OpNop = 0,
    OpUndef = 1,
    OpSourceContinued = 2,
    OpSource = 3,
    OpSourceExtension = 4,
    OpName = 5,
    OpMemberName = 6,
    OpString = 7,
    OpLine = 8,
    OpExtension = 10,
    OpExtInstImport = 11,
    OpExtInst = 12,
    OpMemoryModel = 14,
    OpEntryPoint = 15,
    OpExecutionMode = 16,
    OpCapability = 17,
    OpTypeVoid = 19,
    OpTypeBool = 20,
    OpTypeInt = 21,
    OpTypeFloat = 22,
    OpTypeVector = 23,
    OpTypeMatrix = 24,
    OpTypeImage = 25,
    OpTypeSampler = 26,
    OpTypeSampledImage = 27,
    OpTypeArray = 28,
    OpTypeRuntimeArray = 29,
    OpTypeStruct = 30,
    OpTypeOpaque = 31,
    OpTypePointer = 32,
    OpTypeFunction = 33,
    OpTypeEvent = 34,
    OpTypeDeviceEvent = 35,
    OpTypeReserveId = 36,
    OpTypeQueue = 37,
    OpTypePipe = 38,
    OpTypeForwardPointer = 39,
    OpConstantTrue = 41,
    OpConstantFalse = 42,
    OpConstant = 43,
    OpConstantComposite = 44,
    OpConstantSampler = 45,
    OpConstantNull = 46,
    OpSpecConstantTrue = 48,
    OpSpecConstantFalse = 49,
    OpSpecConstant = 50,
    OpSpecConstantComposite = 51,
    OpSpecConstantOp = 52,
    OpFunction = 54,
    OpFunctionParameter = 55,
    OpFunctionEnd = 56,
    OpFunctionCall = 57,
    OpVariable = 59,
    OpImageTexelPointer = 60,
    OpLoad = 61,
    OpStore = 62,
    OpCopyMemory = 63,
    OpCopyMemorySized = 64,
    OpAccessChain = 65,
    OpInBoundsAccessChain = 66,
    OpPtrAccessChain = 67,
    OpArrayLength = 68,
    OpGenericPtrMemSemantics = 69,
    OpInBoundsPtrAccessChain = 70,
    OpDecorate = 71,
    OpMemberDecorate = 72,
    OpDecorationGroup = 73,
    OpGroupDecorate = 74,
    OpGroupMemberDecorate = 75,
    OpVectorExtractDynamic = 77,
    OpVectorInsertDynamic = 78,
    OpVectorShuffle = 79,
    OpCompositeConstruct = 80,
    OpCompositeExtract = 81,
    OpCompositeInsert = 82,
    OpCopyObject = 83,
    OpTranspose = 84,
    OpSampledImage = 86,
    OpImageSampleImplicitLod = 87,
    OpImageSampleExplicitLod = 88,
    OpImageSampleDrefImplicitLod = 89,
    OpImageSampleDrefExplicitLod = 90,
    OpImageSampleProjImplicitLod = 91,
    OpImageSampleProjExplicitLod = 92,
    OpImageSampleProjDrefImplicitLod = 93,
    OpImageSampleProjDrefExplicitLod = 94,
    OpImageFetch = 95,
    OpImageGather = 96,
    OpImageDrefGather = 97,
    OpImageRead = 98,
    OpImageWrite = 99,
    OpImage = 100,
    OpImageQueryFormat = 101,
    OpImageQueryOrder = 102,
    OpImageQuerySizeLod = 103,
    OpImageQuerySize = 104,
    OpImageQueryLod = 105,
    OpImageQueryLevels = 106,
    OpImageQuerySamples = 107,
    OpConvertFToU = 109,
    OpConvertFToS = 110,
    OpConvertSToF = 111,
    OpConvertUToF = 112,
    OpUConvert = 113,
    OpSConvert = 114,
    OpFConvert = 115,
    OpQuantizeToF16 = 116,
    OpConvertPtrToU = 117,
    OpSatConvertSToU = 118,
    OpSatConvertUToS = 119,
    OpConvertUToPtr = 120,
    OpPtrCastToGeneric = 121,
    OpGenericCastToPtr = 122,
    OpGenericCastToPtrExplicit = 123,
    OpBitcast = 124,
    OpSNegate = 126,
    OpFNegate = 127,
    OpIAdd = 128,
    OpFAdd = 129,
    OpISub = 130,
    OpFSub = 131,
    OpIMul = 132,
    OpFMul = 133,
    OpUDiv = 134,
    OpSDiv = 135,
    OpFDiv = 136,
    OpUMod = 137,
    OpSRem = 138,
    OpSMod = 139,
    OpFRem = 140,
    OpFMod = 141,
    OpVectorTimesScalar = 142,
    OpMatrixTimesScalar = 143,
    OpVectorTimesMatrix = 144,
    OpMatrixTimesVector = 145,
    OpMatrixTimesMatrix = 146,
    OpOuterProduct = 147,
    OpDot = 148,
    OpIAddCarry = 149,
    OpISubBorrow = 150,
    OpUMulExtended = 151,
    OpSMulExtended = 152,
    OpAny = 154,
    OpAll = 155,
    OpIsNan = 156,
    OpIsInf = 157,
    OpIsFinite = 158,
    OpIsNormal = 159,
    OpSignBitSet = 160,
    OpLessOrGreater = 161,
    OpOrdered = 162,
    OpUnordered = 163,
    OpLogicalEqual = 164,
    OpLogicalNotEqual = 165,
    OpLogicalOr = 166,
    OpLogicalAnd = 167,
    OpLogicalNot = 168,
    OpSelect = 169,
    OpIEqual = 170,
    OpINotEqual = 171,
    OpUGreaterThan = 172,
    OpSGreaterThan = 173,
    OpUGreaterThanEqual = 174,
    OpSGreaterThanEqual = 175,
    OpULessThan = 176,
    OpSLessThan = 177,
    OpULessThanEqual = 178,
    OpSLessThanEqual = 179,
    OpFOrdEqual = 180,
    OpFUnordEqual = 181,
    OpFOrdNotEqual = 182,
    OpFUnordNotEqual = 183,
    OpFOrdLessThan = 184,
    OpFUnordLessThan = 185,
    OpFOrdGreaterThan = 186,
    OpFUnordGreaterThan = 187,
    OpFOrdLessThanEqual = 188,
    OpFUnordLessThanEqual = 189,
    OpFOrdGreaterThanEqual = 190,
    OpFUnordGreaterThanEqual = 191,
    OpShiftRightLogical = 194,
    OpShiftRightArithmetic = 195,
    OpShiftLeftLogical = 196,
    OpBitwiseOr = 197,
    OpBitwiseXor = 198,
    OpBitwiseAnd = 199,
    OpNot = 200,
    OpBitFieldInsert = 201,
    OpBitFieldSExtract = 202,
    OpBitFieldUExtract = 203,
    OpBitReverse = 204,
    OpBitCount = 205,
    OpDPdx = 207,
    OpDPdy = 208,
    OpFwidth = 209,
    OpDPdxFine = 210,
    OpDPdyFine = 211,
    OpFwidthFine = 212,
    OpDPdxCoarse = 213,
    OpDPdyCoarse = 214,
    OpFwidthCoarse = 215,
    OpEmitVertex = 218,
    OpEndPrimitive = 219,
    OpEmitStreamVertex = 220,
    OpEndStreamPrimitive = 221,
    OpControlBarrier = 224,
    OpMemoryBarrier = 225,
    OpAtomicLoad = 227,
    OpAtomicStore = 228,
    OpAtomicExchange = 229,
    OpAtomicCompareExchange = 230,
    OpAtomicCompareExchangeWeak = 231,
    OpAtomicIIncrement = 232,
    OpAtomicIDecrement = 233,
    OpAtomicIAdd = 234,
    OpAtomicISub = 235,
    OpAtomicSMin = 236,
    OpAtomicUMin = 237,
    OpAtomicSMax = 238,
    OpAtomicUMax = 239,
    OpAtomicAnd = 240,
    OpAtomicOr = 241,
    OpAtomicXor = 242,
    OpPhi = 245,
    OpLoopMerge = 246,
    OpSelectionMerge = 247,
    OpLabel = 248,
    OpBranch = 249,
    OpBranchConditional = 250,
    OpSwitch = 251,
    OpKill = 252,
    OpReturn = 253,
    OpReturnValue = 254,
    OpUnreachable = 255,
    OpLifetimeStart = 256,
    OpLifetimeStop = 257,
    OpGroupAsyncCopy = 259,
    OpGroupWaitEvents = 260,
    OpGroupAll = 261,
    OpGroupAny = 262,
    OpGroupBroadcast = 263,
    OpGroupIAdd = 264,
    OpGroupFAdd = 265,
    OpGroupFMin = 266,
    OpGroupUMin = 267,
    OpGroupSMin = 268,
    OpGroupFMax = 269,
    OpGroupUMax = 270,
    OpGroupSMax = 271,
    OpReadPipe = 274,
    OpWritePipe = 275,
    OpReservedReadPipe = 276,
    OpReservedWritePipe = 277,
    OpReserveReadPipePackets = 278,
    OpReserveWritePipePackets = 279,
    OpCommitReadPipe = 280,
    OpCommitWritePipe = 281,
    OpIsValidReserveId = 282,
    OpGetNumPipePackets = 283,
    OpGetMaxPipePackets = 284,
    OpGroupReserveReadPipePackets = 285,
    OpGroupReserveWritePipePackets = 286,
    OpGroupCommitReadPipe = 287,
    OpGroupCommitWritePipe = 288,
    OpEnqueueMarker = 291,
    OpEnqueueKernel = 292,
    OpGetKernelNDrangeSubGroupCount = 293,
    OpGetKernelNDrangeMaxSubGroupSize = 294,
    OpGetKernelWorkGroupSize = 295,
    OpGetKernelPreferredWorkGroupSizeMultiple = 296,
    OpRetainEvent = 297,
    OpReleaseEvent = 298,
    OpCreateUserEvent = 299,
    OpIsValidEvent = 300,
    OpSetUserEventStatus = 301,
    OpCaptureEventProfilingInfo = 302,
    OpGetDefaultQueue = 303,
    OpBuildNDRange = 304,
    OpImageSparseSampleImplicitLod = 305,
    OpImageSparseSampleExplicitLod = 306,
    OpImageSparseSampleDrefImplicitLod = 307,
    OpImageSparseSampleDrefExplicitLod = 308,
    OpImageSparseSampleProjImplicitLod = 309,
    OpImageSparseSampleProjExplicitLod = 310,
    OpImageSparseSampleProjDrefImplicitLod = 311,
    OpImageSparseSampleProjDrefExplicitLod = 312,
    OpImageSparseFetch = 313,
    OpImageSparseGather = 314,
    OpImageSparseDrefGather = 315,
    OpImageSparseTexelsResident = 316,
    OpNoLine = 317,
    OpAtomicFlagTestAndSet = 318,
    OpAtomicFlagClear = 319,
    OpImageSparseRead = 320,
    OpSizeOf = 321,
    OpTypePipeStorage = 322,
    OpConstantPipeStorage = 323,
    OpCreatePipeFromPipeStorage = 324,
    OpGetKernelLocalSizeForSubgroupCount = 325,
    OpGetKernelMaxNumSubgroups = 326,
    OpTypeNamedBarrier = 327,
    OpNamedBarrierInitialize = 328,
    OpMemoryNamedBarrier = 329,
    OpModuleProcessed = 330,
    OpExecutionModeId = 331,
    OpDecorateId = 332,
    OpGroupNonUniformElect = 333,
    OpGroupNonUniformAll = 334,
    OpGroupNonUniformAny = 335,
    OpGroupNonUniformAllEqual = 336,
    OpGroupNonUniformBroadcast = 337,
    OpGroupNonUniformBroadcastFirst = 338,
    OpGroupNonUniformBallot = 339,
    OpGroupNonUniformInverseBallot = 340,
    OpGroupNonUniformBallotBitExtract = 341,
    OpGroupNonUniformBallotBitCount = 342,
    OpGroupNonUniformBallotFindLSB = 343,
    OpGroupNonUniformBallotFindMSB = 344,
    OpGroupNonUniformShuffle = 345,
    OpGroupNonUniformShuffleXor = 346,
    OpGroupNonUniformShuffleUp = 347,
    OpGroupNonUniformShuffleDown = 348,
    OpGroupNonUniformIAdd = 349,
    OpGroupNonUniformFAdd = 350,
    OpGroupNonUniformIMul = 351,
    OpGroupNonUniformFMul = 352,
    OpGroupNonUniformSMin = 353,
    OpGroupNonUniformUMin = 354,
    OpGroupNonUniformFMin = 355,
    OpGroupNonUniformSMax = 356,
    OpGroupNonUniformUMax = 357,
    OpGroupNonUniformFMax = 358,
    OpGroupNonUniformBitwiseAnd = 359,
    OpGroupNonUniformBitwiseOr = 360,
    OpGroupNonUniformBitwiseXor = 361,
    OpGroupNonUniformLogicalAnd = 362,
    OpGroupNonUniformLogicalOr = 363,
    OpGroupNonUniformLogicalXor = 364,
    OpGroupNonUniformQuadBroadcast = 365,
    OpGroupNonUniformQuadSwap = 366,
    OpCopyLogical = 400,
    OpPtrEqual = 401,
    OpPtrNotEqual = 402,
    OpPtrDiff = 403,
    OpTerminateInvocation = 4416,
    OpSubgroupBallotKHR = 4421,
    OpSubgroupFirstInvocationKHR = 4422,
    OpSubgroupAllKHR = 4428,
    OpSubgroupAnyKHR = 4429,
    OpSubgroupAllEqualKHR = 4430,
    OpSubgroupReadInvocationKHR = 4432,
    OpTypeRayQueryProvisionalKHR = 4472,
    OpRayQueryInitializeKHR = 4473,
    OpRayQueryTerminateKHR = 4474,
    OpRayQueryGenerateIntersectionKHR = 4475,
    OpRayQueryConfirmIntersectionKHR = 4476,
    OpRayQueryProceedKHR = 4477,
    OpRayQueryGetIntersectionTypeKHR = 4479,
    OpGroupIAddNonUniformAMD = 5000,
    OpGroupFAddNonUniformAMD = 5001,
    OpGroupFMinNonUniformAMD = 5002,
    OpGroupUMinNonUniformAMD = 5003,
    OpGroupSMinNonUniformAMD = 5004,
    OpGroupFMaxNonUniformAMD = 5005,
    OpGroupUMaxNonUniformAMD = 5006,
    OpGroupSMaxNonUniformAMD = 5007,
    OpFragmentMaskFetchAMD = 5011,
    OpFragmentFetchAMD = 5012,
    OpReadClockKHR = 5056,
    OpImageSampleFootprintNV = 5283,
    OpGroupNonUniformPartitionNV = 5296,
    OpWritePackedPrimitiveIndices4x8NV = 5299,
    OpReportIntersectionKHR = 5334,
    OpReportIntersectionNV = 5334,
    OpIgnoreIntersectionKHR = 5335,
    OpIgnoreIntersectionNV = 5335,
    OpTerminateRayKHR = 5336,
    OpTerminateRayNV = 5336,
    OpTraceNV = 5337,
    OpTraceRayKHR = 5337,
    OpTypeAccelerationStructureKHR = 5341,
    OpTypeAccelerationStructureNV = 5341,
    OpExecuteCallableKHR = 5344,
    OpExecuteCallableNV = 5344,
    OpTypeCooperativeMatrixNV = 5358,
    OpCooperativeMatrixLoadNV = 5359,
    OpCooperativeMatrixStoreNV = 5360,
    OpCooperativeMatrixMulAddNV = 5361,
    OpCooperativeMatrixLengthNV = 5362,
    OpBeginInvocationInterlockEXT = 5364,
    OpEndInvocationInterlockEXT = 5365,
    OpDemoteToHelperInvocationEXT = 5380,
    OpIsHelperInvocationEXT = 5381,
    OpSubgroupShuffleINTEL = 5571,
    OpSubgroupShuffleDownINTEL = 5572,
    OpSubgroupShuffleUpINTEL = 5573,
    OpSubgroupShuffleXorINTEL = 5574,
    OpSubgroupBlockReadINTEL = 5575,
    OpSubgroupBlockWriteINTEL = 5576,
    OpSubgroupImageBlockReadINTEL = 5577,
    OpSubgroupImageBlockWriteINTEL = 5578,
    OpSubgroupImageMediaBlockReadINTEL = 5580,
    OpSubgroupImageMediaBlockWriteINTEL = 5581,
    OpUCountLeadingZerosINTEL = 5585,
    OpUCountTrailingZerosINTEL = 5586,
    OpAbsISubINTEL = 5587,
    OpAbsUSubINTEL = 5588,
    OpIAddSatINTEL = 5589,
    OpUAddSatINTEL = 5590,
    OpIAverageINTEL = 5591,
    OpUAverageINTEL = 5592,
    OpIAverageRoundedINTEL = 5593,
    OpUAverageRoundedINTEL = 5594,
    OpISubSatINTEL = 5595,
    OpUSubSatINTEL = 5596,
    OpIMul32x16INTEL = 5597,
    OpUMul32x16INTEL = 5598,
    OpFunctionPointerINTEL = 5600,
    OpFunctionPointerCallINTEL = 5601,
    OpDecorateString = 5632,
    OpDecorateStringGOOGLE = 5632,
    OpMemberDecorateString = 5633,
    OpMemberDecorateStringGOOGLE = 5633,
    OpVmeImageINTEL = 5699,
    OpTypeVmeImageINTEL = 5700,
    OpTypeAvcImePayloadINTEL = 5701,
    OpTypeAvcRefPayloadINTEL = 5702,
    OpTypeAvcSicPayloadINTEL = 5703,
    OpTypeAvcMcePayloadINTEL = 5704,
    OpTypeAvcMceResultINTEL = 5705,
    OpTypeAvcImeResultINTEL = 5706,
    OpTypeAvcImeResultSingleReferenceStreamoutINTEL = 5707,
    OpTypeAvcImeResultDualReferenceStreamoutINTEL = 5708,
    OpTypeAvcImeSingleReferenceStreaminINTEL = 5709,
    OpTypeAvcImeDualReferenceStreaminINTEL = 5710,
    OpTypeAvcRefResultINTEL = 5711,
    OpTypeAvcSicResultINTEL = 5712,
    OpSubgroupAvcMceGetDefaultInterBaseMultiReferencePenaltyINTEL = 5713,
    OpSubgroupAvcMceSetInterBaseMultiReferencePenaltyINTEL = 5714,
    OpSubgroupAvcMceGetDefaultInterShapePenaltyINTEL = 5715,
    OpSubgroupAvcMceSetInterShapePenaltyINTEL = 5716,
    OpSubgroupAvcMceGetDefaultInterDirectionPenaltyINTEL = 5717,
    OpSubgroupAvcMceSetInterDirectionPenaltyINTEL = 5718,
    OpSubgroupAvcMceGetDefaultIntraLumaShapePenaltyINTEL = 5719,
    OpSubgroupAvcMceGetDefaultInterMotionVectorCostTableINTEL = 5720,
    OpSubgroupAvcMceGetDefaultHighPenaltyCostTableINTEL = 5721,
    OpSubgroupAvcMceGetDefaultMediumPenaltyCostTableINTEL = 5722,
    OpSubgroupAvcMceGetDefaultLowPenaltyCostTableINTEL = 5723,
    OpSubgroupAvcMceSetMotionVectorCostFunctionINTEL = 5724,
    OpSubgroupAvcMceGetDefaultIntraLumaModePenaltyINTEL = 5725,
    OpSubgroupAvcMceGetDefaultNonDcLumaIntraPenaltyINTEL = 5726,
    OpSubgroupAvcMceGetDefaultIntraChromaModeBasePenaltyINTEL = 5727,
    OpSubgroupAvcMceSetAcOnlyHaarINTEL = 5728,
    OpSubgroupAvcMceSetSourceInterlacedFieldPolarityINTEL = 5729,
    OpSubgroupAvcMceSetSingleReferenceInterlacedFieldPolarityINTEL = 5730,
    OpSubgroupAvcMceSetDualReferenceInterlacedFieldPolaritiesINTEL = 5731,
    OpSubgroupAvcMceConvertToImePayloadINTEL = 5732,
    OpSubgroupAvcMceConvertToImeResultINTEL = 5733,
    OpSubgroupAvcMceConvertToRefPayloadINTEL = 5734,
    OpSubgroupAvcMceConvertToRefResultINTEL = 5735,
    OpSubgroupAvcMceConvertToSicPayloadINTEL = 5736,
    OpSubgroupAvcMceConvertToSicResultINTEL = 5737,
    OpSubgroupAvcMceGetMotionVectorsINTEL = 5738,
    OpSubgroupAvcMceGetInterDistortionsINTEL = 5739,
    OpSubgroupAvcMceGetBestInterDistortionsINTEL = 5740,
    OpSubgroupAvcMceGetInterMajorShapeINTEL = 5741,
    OpSubgroupAvcMceGetInterMinorShapeINTEL = 5742,
    OpSubgroupAvcMceGetInterDirectionsINTEL = 5743,
    OpSubgroupAvcMceGetInterMotionVectorCountINTEL = 5744,
    OpSubgroupAvcMceGetInterReferenceIdsINTEL = 5745,
    OpSubgroupAvcMceGetInterReferenceInterlacedFieldPolaritiesINTEL = 5746,
    OpSubgroupAvcImeInitializeINTEL = 5747,
    OpSubgroupAvcImeSetSingleReferenceINTEL = 5748,
    OpSubgroupAvcImeSetDualReferenceINTEL = 5749,
    OpSubgroupAvcImeRefWindowSizeINTEL = 5750,
    OpSubgroupAvcImeAdjustRefOffsetINTEL = 5751,
    OpSubgroupAvcImeConvertToMcePayloadINTEL = 5752,
    OpSubgroupAvcImeSetMaxMotionVectorCountINTEL = 5753,
    OpSubgroupAvcImeSetUnidirectionalMixDisableINTEL = 5754,
    OpSubgroupAvcImeSetEarlySearchTerminationThresholdINTEL = 5755,
    OpSubgroupAvcImeSetWeightedSadINTEL = 5756,
    OpSubgroupAvcImeEvaluateWithSingleReferenceINTEL = 5757,
    OpSubgroupAvcImeEvaluateWithDualReferenceINTEL = 5758,
    OpSubgroupAvcImeEvaluateWithSingleReferenceStreaminINTEL = 5759,
    OpSubgroupAvcImeEvaluateWithDualReferenceStreaminINTEL = 5760,
    OpSubgroupAvcImeEvaluateWithSingleReferenceStreamoutINTEL = 5761,
    OpSubgroupAvcImeEvaluateWithDualReferenceStreamoutINTEL = 5762,
    OpSubgroupAvcImeEvaluateWithSingleReferenceStreaminoutINTEL = 5763,
    OpSubgroupAvcImeEvaluateWithDualReferenceStreaminoutINTEL = 5764,
    OpSubgroupAvcImeConvertToMceResultINTEL = 5765,
    OpSubgroupAvcImeGetSingleReferenceStreaminINTEL = 5766,
    OpSubgroupAvcImeGetDualReferenceStreaminINTEL = 5767,
    OpSubgroupAvcImeStripSingleReferenceStreamoutINTEL = 5768,
    OpSubgroupAvcImeStripDualReferenceStreamoutINTEL = 5769,
    OpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeMotionVectorsINTEL = 5770,
    OpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeDistortionsINTEL = 5771,
    OpSubgroupAvcImeGetStreamoutSingleReferenceMajorShapeReferenceIdsINTEL = 5772,
    OpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeMotionVectorsINTEL = 5773,
    OpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeDistortionsINTEL = 5774,
    OpSubgroupAvcImeGetStreamoutDualReferenceMajorShapeReferenceIdsINTEL = 5775,
    OpSubgroupAvcImeGetBorderReachedINTEL = 5776,
    OpSubgroupAvcImeGetTruncatedSearchIndicationINTEL = 5777,
    OpSubgroupAvcImeGetUnidirectionalEarlySearchTerminationINTEL = 5778,
    OpSubgroupAvcImeGetWeightingPatternMinimumMotionVectorINTEL = 5779,
    OpSubgroupAvcImeGetWeightingPatternMinimumDistortionINTEL = 5780,
    OpSubgroupAvcFmeInitializeINTEL = 5781,
    OpSubgroupAvcBmeInitializeINTEL = 5782,
    OpSubgroupAvcRefConvertToMcePayloadINTEL = 5783,
    OpSubgroupAvcRefSetBidirectionalMixDisableINTEL = 5784,
    OpSubgroupAvcRefSetBilinearFilterEnableINTEL = 5785,
    OpSubgroupAvcRefEvaluateWithSingleReferenceINTEL = 5786,
    OpSubgroupAvcRefEvaluateWithDualReferenceINTEL = 5787,
    OpSubgroupAvcRefEvaluateWithMultiReferenceINTEL = 5788,
    OpSubgroupAvcRefEvaluateWithMultiReferenceInterlacedINTEL = 5789,
    OpSubgroupAvcRefConvertToMceResultINTEL = 5790,
    OpSubgroupAvcSicInitializeINTEL = 5791,
    OpSubgroupAvcSicConfigureSkcINTEL = 5792,
    OpSubgroupAvcSicConfigureIpeLumaINTEL = 5793,
    OpSubgroupAvcSicConfigureIpeLumaChromaINTEL = 5794,
    OpSubgroupAvcSicGetMotionVectorMaskINTEL = 5795,
    OpSubgroupAvcSicConvertToMcePayloadINTEL = 5796,
    OpSubgroupAvcSicSetIntraLumaShapePenaltyINTEL = 5797,
    OpSubgroupAvcSicSetIntraLumaModeCostFunctionINTEL = 5798,
    OpSubgroupAvcSicSetIntraChromaModeCostFunctionINTEL = 5799,
    OpSubgroupAvcSicSetBilinearFilterEnableINTEL = 5800,
    OpSubgroupAvcSicSetSkcForwardTransformEnableINTEL = 5801,
    OpSubgroupAvcSicSetBlockBasedRawSkipSadINTEL = 5802,
    OpSubgroupAvcSicEvaluateIpeINTEL = 5803,
    OpSubgroupAvcSicEvaluateWithSingleReferenceINTEL = 5804,
    OpSubgroupAvcSicEvaluateWithDualReferenceINTEL = 5805,
    OpSubgroupAvcSicEvaluateWithMultiReferenceINTEL = 5806,
    OpSubgroupAvcSicEvaluateWithMultiReferenceInterlacedINTEL = 5807,
    OpSubgroupAvcSicConvertToMceResultINTEL = 5808,
    OpSubgroupAvcSicGetIpeLumaShapeINTEL = 5809,
    OpSubgroupAvcSicGetBestIpeLumaDistortionINTEL = 5810,
    OpSubgroupAvcSicGetBestIpeChromaDistortionINTEL = 5811,
    OpSubgroupAvcSicGetPackedIpeLumaModesINTEL = 5812,
    OpSubgroupAvcSicGetIpeChromaModeINTEL = 5813,
    OpSubgroupAvcSicGetPackedSkcLumaCountThresholdINTEL = 5814,
    OpSubgroupAvcSicGetPackedSkcLumaSumThresholdINTEL = 5815,
    OpSubgroupAvcSicGetInterRawSadsINTEL = 5816,
    OpLoopControlINTEL = 5887,
    OpReadPipeBlockingINTEL = 5946,
    OpWritePipeBlockingINTEL = 5947,
    OpFPGARegINTEL = 5949,
    OpRayQueryGetRayTMinKHR = 6016,
    OpRayQueryGetRayFlagsKHR = 6017,
    OpRayQueryGetIntersectionTKHR = 6018,
    OpRayQueryGetIntersectionInstanceCustomIndexKHR = 6019,
    OpRayQueryGetIntersectionInstanceIdKHR = 6020,
    OpRayQueryGetIntersectionInstanceShaderBindingTableRecordOffsetKHR = 6021,
    OpRayQueryGetIntersectionGeometryIndexKHR = 6022,
    OpRayQueryGetIntersectionPrimitiveIndexKHR = 6023,
    OpRayQueryGetIntersectionBarycentricsKHR = 6024,
    OpRayQueryGetIntersectionFrontFaceKHR = 6025,
    OpRayQueryGetIntersectionCandidateAABBOpaqueKHR = 6026,
    OpRayQueryGetIntersectionObjectRayDirectionKHR = 6027,
    OpRayQueryGetIntersectionObjectRayOriginKHR = 6028,
    OpRayQueryGetWorldRayDirectionKHR = 6029,
    OpRayQueryGetWorldRayOriginKHR = 6030,
    OpRayQueryGetIntersectionObjectToWorldKHR = 6031,
    OpRayQueryGetIntersectionWorldToObjectKHR = 6032,
    OpAtomicFAddEXT = 6035
  };


  namespace Reflect{
    enum class Result {
      Unknown = -1,
      Success = 0,
      NotReady,

      ParseFailed,
      AllocFailed,
      RangeExceeded,
      NullPointerDereference,
      InternalError,
      CountMismatch,
      ElementNotFound,

      InvalidCodeSize,
      InvalidMagicNumber,
      UnexpectedEof,
      InvalidIdReference,
      SetNumberOverflow,
      InvalidStorageClass,
      Recursion,
      InvalidInstruction,
      UnexpectedBlockData,
      InvalidBlockMemberReference,
      InvalidEntryPoint,
      InvalidExecutionMode
    };

    class Type : public Core::BitFlags<Bits::ReflectType> {
    public:
      using Core::BitFlags<Bits::ReflectType>::BitFlags;

      using bit_type = Bits::ReflectType;
      using enum_type = Type;

      VK_inline_bitflag(Undefined);
      VK_inline_bitflag(Void);
      VK_inline_bitflag(Bool);
      VK_inline_bitflag(Int);
      VK_inline_bitflag(Float);
      VK_inline_bitflag(Vector);
      VK_inline_bitflag(Matrix);
      VK_inline_bitflag(ExternalImage);
      VK_inline_bitflag(ExternalSampler);
      VK_inline_bitflag(ExternalSampledImage);
      VK_inline_bitflag(ExternalBlock);
      VK_inline_bitflag(ExternalMask);
      VK_inline_bitflag(Struct);
      VK_inline_bitflag(Array);
    };

    class Decoration : public Core::BitFlags<Bits::ReflectDecoration> {
    public:
      using Core::BitFlags<Bits::ReflectDecoration>::BitFlags;

      using bit_type = Bits::ReflectDecoration;
      using enum_type = Decoration;

      VK_inline_bitflag(None);
      VK_inline_bitflag(Block);
      VK_inline_bitflag(BufferBlock);
      VK_inline_bitflag(RowMajor);
      VK_inline_bitflag(ColumnMajor);
      VK_inline_bitflag(BuiltIn);
      VK_inline_bitflag(Noperspective);
      VK_inline_bitflag(Flat);
      VK_inline_bitflag(NonWritable);
    };

    enum class ResourceType {
      Undefined = 0x00000000,
      Sampler = 0x00000001,
      Cbv = 0x00000002,
      Srv = 0x00000004,
      Uav = 0x00000008,
    };

    class Variable : public Core::BitFlags<Bits::ReflectVariable>{
    public:
      using enum_type = Variable;
      using bit_type = Bits::ReflectVariable;

      using Core::BitFlags<Bits::ReflectVariable>::BitFlags;

      VK_inline_bitflag(None);
      VK_inline_bitflag(Unused);
    };

    enum class Format {
      Undefined          =   0, // = VK_FORMAT_UNDEFINED
      R32UInt            =  98, // = VK_FORMAT_R32_UINT
      R32SInt            =  99, // = VK_FORMAT_R32_SINT
      R32SFloat          = 100, // = VK_FORMAT_R32_SFLOAT
      R32G32UInt         = 101, // = VK_FORMAT_R32G32_UINT
      R32G32SInt         = 102, // = VK_FORMAT_R32G32_SINT
      R32G32SFloat       = 103, // = VK_FORMAT_R32G32_SFLOAT
      R32G32B32UInt      = 104, // = VK_FORMAT_R32G32B32_UINT
      R32G32B32SInt      = 105, // = VK_FORMAT_R32G32B32_SINT
      R32G32B32SFloat    = 106, // = VK_FORMAT_R32G32B32_SFLOAT
      R32G32B32A32UInt   = 107, // = VK_FORMAT_R32G32B32A32_UINT
      R32G32B32A32SInt   = 108, // = VK_FORMAT_R32G32B32A32_SINT
      R32G32B32A32SFloat = 109, // = VK_FORMAT_R32G32B32A32_SFLOAT
    };

    enum class DescriptorType{
      Sampler                =  0, // = VK_DESCRIPTOR_TYPE_SAMPLER
      CombinedImageSampler =  1, // = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
      SampledImage          =  2, // = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
      StorageImage          =  3, // = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
      UniformTexelBuffer   =  4, // = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
      StorageTexelBuffer   =  5, // = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
      UniformBuffer         =  6, // = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
      StorageBuffer         =  7, // = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
      UniformBufferDynamic =  8, // = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
      StorageBufferDynamic =  9, // = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
      InputAttachment       = 10, // = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
    };

    class ShaderStage : public Core::BitFlags<Bits::ReflectShaderStage>{
    public:
      using enum_type = ShaderStage;
      using bit_type = Bits::ReflectShaderStage;

      using Core::BitFlags<Bits::ReflectShaderStage>::BitFlags;

      VK_inline_bitflag(Vertex);
      VK_inline_bitflag(Geometry);
      VK_inline_bitflag(TessellationControl);
      VK_inline_bitflag(TessellationEvaluation);
      VK_inline_bitflag(Fragment);
      VK_inline_bitflag(Compute);
    };

    enum class Generator {
      KhronosLLVMSpirVTranslator         = 6,
      KhronosSpirVToolsAssembler         = 7,
      KhronosGLSLangReferenceFrontEnd    = 8,
      GoogleShaderCOverGLSLang           = 13,
      GoogleSpiregg                        = 14,
      GoogleRspirV                         = 15,
      XLegendMesaMesairSpirVTranslator = 16,
      KhronosSpirVToolsLinker            = 17,
      WineVkd3dShaderCompiler            = 18,
      ClayClayShaderCompiler             = 19,
    };
  }
}



#endif //VALKYRIE_SPIRV_ENUMS_HPP
