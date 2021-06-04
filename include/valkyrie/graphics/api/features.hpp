//
// Created by maxwe on 2021-05-29.
//

#ifndef VALKYRIE_GRAPHICS_API_FEATURES_HPP
#define VALKYRIE_GRAPHICS_API_FEATURES_HPP

#include "vulkan.hpp"
#include "device.hpp"


namespace valkyrie::graphics::api{

  struct physical_device_features;


  
  namespace features{
    struct physical_device_features2 : out_structure<physical_device_features2>{
      bool32    robustBufferAccess;
      bool32    fullDrawIndexUint32;
      bool32    imageCubeArray;
      bool32    independentBlend;
      bool32    geometryShader;
      bool32    tessellationShader;
      bool32    sampleRateShading;
      bool32    dualSrcBlend;
      bool32    logicOp;
      bool32    multiDrawIndirect;
      bool32    drawIndirectFirstInstance;
      bool32    depthClamp;
      bool32    depthBiasClamp;
      bool32    fillModeNonSolid;
      bool32    depthBounds;
      bool32    wideLines;
      bool32    largePoints;
      bool32    alphaToOne;
      bool32    multiViewport;
      bool32    samplerAnisotropy;
      bool32    textureCompressionETC2;
      bool32    textureCompressionASTC_LDR;
      bool32    textureCompressionBC;
      bool32    occlusionQueryPrecise;
      bool32    pipelineStatisticsQuery;
      bool32    vertexPipelineStoresAndAtomics;
      bool32    fragmentStoresAndAtomics;
      bool32    shaderTessellationAndGeometryPointSize;
      bool32    shaderImageGatherExtended;
      bool32    shaderStorageImageExtendedFormats;
      bool32    shaderStorageImageMultisample;
      bool32    shaderStorageImageReadWithoutFormat;
      bool32    shaderStorageImageWriteWithoutFormat;
      bool32    shaderUniformBufferArrayDynamicIndexing;
      bool32    shaderSampledImageArrayDynamicIndexing;
      bool32    shaderStorageBufferArrayDynamicIndexing;
      bool32    shaderStorageImageArrayDynamicIndexing;
      bool32    shaderClipDistance;
      bool32    shaderCullDistance;
      bool32    shaderFloat64;
      bool32    shaderInt64;
      bool32    shaderInt16;
      bool32    shaderResourceResidency;
      bool32    shaderResourceMinLod;
      bool32    sparseBinding;
      bool32    sparseResidencyBuffer;
      bool32    sparseResidencyImage2D;
      bool32    sparseResidencyImage3D;
      bool32    sparseResidency2Samples;
      bool32    sparseResidency4Samples;
      bool32    sparseResidency8Samples;
      bool32    sparseResidency16Samples;
      bool32    sparseResidencyAliased;
      bool32    variableMultisampleRate;
      bool32    inheritedQueries;
    };
  }
  
  template <>
  struct impl::structure_type_enum<features::physical_device_features2>{
    VK_constant structure_type value = structure_type::physical_device_features2;
  };

#pragma region macro definitions
#define PP_VK_impl_LIST_FEATURE(ft) features::ft ft;
#define PP_VK_impl_API_FEATURES_ENUM_2(type, ext) physical_device_##type##_features_##ext
#define PP_VK_impl_API_FEATURES_ENUM_1(type) physical_device_##type##_features
#define PP_VK_impl_API_FEATURES_TYPE_2(type, ext) type##_##ext
#define PP_VK_impl_API_FEATURES_TYPE_1(type) type
#define PP_VK_impl_API_FEATURE_DECL(val) bool32 val;
#define PP_VK_impl_API_FEATURES_BODY(...) { VK_foreach(PP_VK_impl_API_FEATURE_DECL, ##__VA_ARGS__) }
#define PP_VK_impl_API_FEATURES_TYPE(...) VK_dispatch(PP_VK_impl_API_FEATURES_TYPE_, __VA_ARGS__)
#define PP_VK_impl_API_FEATURES_ENUM(...) VK_dispatch(PP_VK_impl_API_FEATURES_ENUM_, __VA_ARGS__)
  
#define VK_api_features(...) \
  namespace features { struct PP_VK_impl_API_FEATURES_TYPE(__VA_ARGS__); } \
  template <> struct impl::structure_type_enum<features::PP_VK_impl_API_FEATURES_TYPE(__VA_ARGS__)>{\
    VK_constant structure_type value = structure_type::PP_VK_impl_API_FEATURES_ENUM(__VA_ARGS__);   \
  };                          \
  struct features::PP_VK_impl_API_FEATURES_TYPE(__VA_ARGS__) \
    : out_structure<PP_VK_impl_API_FEATURES_TYPE(__VA_ARGS__),             \
                    features::physical_device_features2,                 \
                    device_create_info> PP_VK_impl_API_FEATURES_BODY
#pragma endregion
  
#pragma region feature declarations
  VK_api_features(x4444_formats, EXT)(
    formatA4R4G4B4,
    formatA4B4G4R4
  );
  VK_api_features(astc_decode, EXT)(
    decodeModeSharedExponent
  );
  VK_api_features(acceleration_structure, KHR)(
    accelerationStructure,
    accelerationStructureCaptureReplay,
    accelerationStructureIndirectBuild,
    accelerationStructureHostCommands,
    descriptorBindingAccelerationStructureUpdateAfterBind
  );
  VK_api_features(blend_operation_advanced, EXT)(
    advancedBlendCoherentOperations
  );
  VK_api_features(coherent_memory, AMD)(
    deviceCoherentMemory
  );
  VK_api_features(color_write_enable, EXT)(
    colorWriteEnable
  );
  VK_api_features(compute_shader_derivatives, NV)(
    computeDerivativeGroupQuads,
    computeDerivativeGroupLinear
  );
  VK_api_features(conditional_rendering, EXT)(
    conditionalRendering,
    inheritedConditionalRendering
  );
  VK_api_features(cooperative_matrix, NV)(
    cooperativeMatrix,
    cooperativeMatrixRobustBufferAccess
  );
  VK_api_features(corner_sampled_image, NV)(
    cornerSampledImage
  );
  VK_api_features(coverage_reduction_mode, NV)(
    coverageReductionMode
  );
  VK_api_features(custom_border_color, EXT)(
    customBorderColors,
    customBorderColorWithoutFormat
  );
  VK_api_features(dedicated_allocation_image_aliasing, NV)(
    dedicatedAllocationImageAliasing
  );
  VK_api_features(depth_clip_enable, EXT)(
    depthClipEnable
  );
  VK_api_features(device_generated_commands, NV)(
    deviceGeneratedCommands
  );
  VK_api_features(device_memory_report, EXT)(
    deviceMemoryReport
  );
  VK_api_features(diagnostics_config, NV)(
    diagnosticsConfig
  );
  VK_api_features(exclusive_scissor, NV)(
    exclusiveScissor
  );
  VK_api_features(extended_dynamic_state2, EXT)(
    extendedDynamicState2,
    extendedDynamicState2LogicOp,
    extendedDynamicState2PatchControlPoints
  );
  VK_api_features(extended_dynamic_state, EXT)(
    extendedDynamicState
  );
  VK_api_features(fragment_density_map2, EXT)(
    fragmentDensityMapDeferred
  );
  VK_api_features(fragment_density_map, EXT)(
    fragmentDensityMap,
    fragmentDensityMapDynamic,
    fragmentDensityMapNonSubsampledImages
  );
  VK_api_features(fragment_shader_barycentric, NV)(
    fragmentShaderBarycentric
  );
  VK_api_features(fragment_shader_interlock, EXT)(
    fragmentShaderSampleInterlock,
    fragmentShaderPixelInterlock,
    fragmentShaderShadingRateInterlock
  );
  VK_api_features(fragment_shading_rate_enums, NV)(
    fragmentShadingRateEnums,
    supersampleFragmentShadingRates,
    noInvocationFragmentShadingRates
  );
  VK_api_features(fragment_shading_rate, KHR)(
    pipelineFragmentShadingRate,
    primitiveFragmentShadingRate,
    attachmentFragmentShadingRate
  );
  VK_api_features(image_robustness, EXT)(
    robustImageAccess
  );
  VK_api_features(index_type_uint8, EXT)(
    indexTypeUint8
  );
  VK_api_features(inherited_viewport_scissor, NV)(
    inheritedViewportScissor2D
  );
  VK_api_features(inline_uniform_block, EXT)(
    inlineUniformBlock,
    descriptorBindingInlineUniformBlockUpdateAfterBind
  );
  VK_api_features(line_rasterization, EXT)(
    rectangularLines,
    bresenhamLines,
    smoothLines,
    stippledRectangularLines,
    stippledBresenhamLines,
    stippledSmoothLines
  );
  VK_api_features(memory_priority, EXT)(
    memoryPriority
  );
  VK_api_features(mesh_shader, NV)(
    taskShader,
    meshShader
  );
  VK_api_features(mutable_descriptor_type, VALVE)(
    mutableDescriptorType
  );
  VK_api_features(performance_query, KHR)(
    performanceCounterQueryPools,
    performanceCounterMultipleQueryPools
  );
  VK_api_features(pipeline_creation_cache_control, EXT)(
    pipelineCreationCacheControl
  );
  VK_api_features(pipeline_executable_properties, KHR)(
    pipelineExecutableInfo
  );
  VK_api_features(portability_subset, KHR)(
    constantAlphaColorBlendFactors,
    events,
    imageViewFormatReinterpretation,
    imageViewFormatSwizzle,
    imageView2DOn3DImage,
    multisampleArrayImage,
    mutableComparisonSamplers,
    pointPolygons,
    samplerMipLodBias,
    separateStencilMaskRef,
    shaderSampleRateInterpolationFunctions,
    tessellationIsolines,
    tessellationPointMode,
    triangleFans,
    vertexAttributeAccessBeyondStride
  );
  VK_api_features(private_data, EXT)(
    privateData
  );
  VK_api_features(provoking_vertex, EXT)(
    provokingVertexLast,
    transformFeedbackPreservesProvokingVertex
  );
  VK_api_features(ray_query, KHR)(
    rayQuery
  );
  VK_api_features(ray_tracing_pipeline, KHR)(
    rayTracingPipeline,
    rayTracingPipelineShaderGroupHandleCaptureReplay,
    rayTracingPipelineShaderGroupHandleCaptureReplayMixed,
    rayTracingPipelineTraceRaysIndirect,
    rayTraversalPrimitiveCulling
  );
  VK_api_features(representative_fragment_test, NV)(
    representativeFragmentTest
  );
  VK_api_features(robustness2, EXT)(
    robustBufferAccess2,
    robustImageAccess2,
    nullDescriptor
  );
  VK_api_features(shader_atomic_float, EXT)(
    shaderBufferFloat32Atomics,
    shaderBufferFloat32AtomicAdd,
    shaderBufferFloat64Atomics,
    shaderBufferFloat64AtomicAdd,
    shaderSharedFloat32Atomics,
    shaderSharedFloat32AtomicAdd,
    shaderSharedFloat64Atomics,
    shaderSharedFloat64AtomicAdd,
    shaderImageFloat32Atomics,
    shaderImageFloat32AtomicAdd,
    sparseImageFloat32Atomics,
    sparseImageFloat32AtomicAdd
  );
  VK_api_features(shader_clock, KHR)(
    shaderSubgroupClock,
    shaderDeviceClock
  );
  VK_api_features(shader_demote_to_helper_invocation, EXT)(
    shaderDemoteToHelperInvocation
  );
  VK_api_features(shader_image_atomic_int64, EXT)(
    shaderImageInt64Atomics,
    sparseImageInt64Atomics
  );
  VK_api_features(shader_image_footprint, NV)(
    imageFootprint
  );
  VK_api_features(shader_integer_functions2, INTEL)(
    shaderIntegerFunctions2
  );
  VK_api_features(shader_sm_builtins, NV)(
    shaderSMBuiltins
  );
  VK_api_features(shader_terminate_invocation, KHR)(
    shaderTerminateInvocation
  );
  VK_api_features(shading_rate_image, NV)(
    shadingRateImage,
    shadingRateCoarseSampleOrder
  );
  VK_api_features(subgroup_size_control, EXT)(
    subgroupSizeControl,
    computeFullSubgroups
  );
  VK_api_features(synchronization2, KHR)(
    synchronization2
  );
  VK_api_features(texel_buffer_alignment, EXT)(
    texelBufferAlignment
  );
  VK_api_features(texture_compression_astc_hdr, EXT)(
    textureCompressionASTC_HDR
  );
  VK_api_features(transform_feedback, EXT)(
    transformFeedback,
    geometryStreams
  );
  VK_api_features(vertex_attribute_divisor, EXT)(
    vertexAttributeInstanceRateDivisor,
    vertexAttributeInstanceRateZeroDivisor
  );
  VK_api_features(vertex_input_dynamic_state, EXT)(
    vertexInputDynamicState
  );
  VK_api_features(vulkan_1_1)(
    storageBuffer16BitAccess,
    uniformAndStorageBuffer16BitAccess,
    storagePushConstant16,
    storageInputOutput16,
    multiview,
    multiviewGeometryShader,
    multiviewTessellationShader,
    variablePointersStorageBuffer,
    variablePointers,
    protectedMemory,
    samplerYcbcrConversion,
    shaderDrawParameters
  );
  VK_api_features(vulkan_1_2)(
    samplerMirrorClampToEdge,
    drawIndirectCount,
    storageBuffer8BitAccess,
    uniformAndStorageBuffer8BitAccess,
    storagePushConstant8,
    shaderBufferInt64Atomics,
    shaderSharedInt64Atomics,
    shaderFloat16,
    shaderInt8,
    descriptorIndexing,
    shaderInputAttachmentArrayDynamicIndexing,
    shaderUniformTexelBufferArrayDynamicIndexing,
    shaderStorageTexelBufferArrayDynamicIndexing,
    shaderUniformBufferArrayNonUniformIndexing,
    shaderSampledImageArrayNonUniformIndexing,
    shaderStorageBufferArrayNonUniformIndexing,
    shaderStorageImageArrayNonUniformIndexing,
    shaderInputAttachmentArrayNonUniformIndexing,
    shaderUniformTexelBufferArrayNonUniformIndexing,
    shaderStorageTexelBufferArrayNonUniformIndexing,
    descriptorBindingUniformBufferUpdateAfterBind,
    descriptorBindingSampledImageUpdateAfterBind,
    descriptorBindingStorageImageUpdateAfterBind,
    descriptorBindingStorageBufferUpdateAfterBind,
    descriptorBindingUniformTexelBufferUpdateAfterBind,
    descriptorBindingStorageTexelBufferUpdateAfterBind,
    descriptorBindingUpdateUnusedWhilePending,
    descriptorBindingPartiallyBound,
    descriptorBindingVariableDescriptorCount,
    runtimeDescriptorArray,
    samplerFilterMinmax,
    scalarBlockLayout,
    imagelessFramebuffer,
    uniformBufferStandardLayout,
    shaderSubgroupExtendedTypes,
    separateDepthStencilLayouts,
    hostQueryReset,
    timelineSemaphore,
    bufferDeviceAddress,
    bufferDeviceAddressCaptureReplay,
    bufferDeviceAddressMultiDevice,
    vulkanMemoryModel,
    vulkanMemoryModelDeviceScope,
    vulkanMemoryModelAvailabilityVisibilityChains,
    shaderOutputViewportIndex,
    shaderOutputLayer,
    subgroupBroadcastDynamicId
  );
  VK_api_features(workgroup_memory_explicit_layout, KHR)(
    workgroupMemoryExplicitLayout,
    workgroupMemoryExplicitLayoutScalarBlockLayout,
    workgroupMemoryExplicitLayout8BitAccess,
    workgroupMemoryExplicitLayout16BitAccess
  );
  VK_api_features(ycbcr_2plane_444_formats, EXT)(
    ycbcr2plane444Formats
  );
  VK_api_features(ycbcr_image_arrays, EXT)(
    ycbcrImageArrays
  );
  VK_api_features(zero_initialize_workgroup_memory, KHR)(
    zeroInitializeWorkgroupMemory
  );
#pragma endregion

  
  struct physical_device_features{
    features::physical_device_features2 vulkan_1_0;
    VK_foreach(PP_VK_impl_LIST_FEATURE,
               x4444_formats_EXT,
               astc_decode_EXT,
               acceleration_structure_KHR,
               blend_operation_advanced_EXT,
               coherent_memory_AMD,
               color_write_enable_EXT,
               compute_shader_derivatives_NV,
               conditional_rendering_EXT,
               cooperative_matrix_NV,
               corner_sampled_image_NV,
               coverage_reduction_mode_NV,
               custom_border_color_EXT,
               dedicated_allocation_image_aliasing_NV,
               depth_clip_enable_EXT,
               device_generated_commands_NV,
               device_memory_report_EXT,
               diagnostics_config_NV,
               exclusive_scissor_NV,
               extended_dynamic_state2_EXT,
               extended_dynamic_state_EXT,
               fragment_density_map2_EXT,
               fragment_density_map_EXT,
               fragment_shader_barycentric_NV,
               fragment_shader_interlock_EXT,
               fragment_shading_rate_enums_NV,
               fragment_shading_rate_KHR,
               image_robustness_EXT,
               index_type_uint8_EXT,
               inherited_viewport_scissor_NV,
               inline_uniform_block_EXT,
               line_rasterization_EXT,
               memory_priority_EXT,
               mesh_shader_NV,
               mutable_descriptor_type_VALVE,
               performance_query_KHR,
               pipeline_creation_cache_control_EXT,
               pipeline_executable_properties_KHR,
               portability_subset_KHR,
               private_data_EXT,
               provoking_vertex_EXT,
               ray_query_KHR,
               ray_tracing_pipeline_KHR,
               representative_fragment_test_NV,
               robustness2_EXT,
               shader_atomic_float_EXT,
               shader_clock_KHR,
               shader_demote_to_helper_invocation_EXT,
               shader_image_atomic_int64_EXT,
               shader_image_footprint_NV,
               shader_integer_functions2_INTEL,
               shader_sm_builtins_NV,
               shader_terminate_invocation_KHR,
               shading_rate_image_NV,
               subgroup_size_control_EXT,
               synchronization2_KHR,
               texel_buffer_alignment_EXT,
               texture_compression_astc_hdr_EXT,
               transform_feedback_EXT,
               vertex_attribute_divisor_EXT,
               vertex_input_dynamic_state_EXT,
               vulkan_1_1,
               vulkan_1_2,
               workgroup_memory_explicit_layout_KHR,
               ycbcr_2plane_444_formats_EXT,
               ycbcr_image_arrays_EXT,
               zero_initialize_workgroup_memory_KHR)
    
    physical_device_features() noexcept {
      ycbcr_image_arrays_EXT.link(zero_initialize_workgroup_memory_KHR);
      ycbcr_2plane_444_formats_EXT.link(ycbcr_image_arrays_EXT);
      workgroup_memory_explicit_layout_KHR.link(ycbcr_2plane_444_formats_EXT);
      vertex_input_dynamic_state_EXT.link(workgroup_memory_explicit_layout_KHR);
      vertex_attribute_divisor_EXT.link(vertex_input_dynamic_state_EXT);
      texture_compression_astc_hdr_EXT.link(vertex_attribute_divisor_EXT);
      texel_buffer_alignment_EXT.link(texture_compression_astc_hdr_EXT);
      transform_feedback_EXT.link(texel_buffer_alignment_EXT);
      synchronization2_KHR.link(transform_feedback_EXT);
      subgroup_size_control_EXT.link(synchronization2_KHR);
      shading_rate_image_NV.link(subgroup_size_control_EXT);
      shader_terminate_invocation_KHR.link(shading_rate_image_NV);
      shader_sm_builtins_NV.link(shader_terminate_invocation_KHR);
      shader_integer_functions2_INTEL.link(shader_sm_builtins_NV);
      shader_image_footprint_NV.link(shader_integer_functions2_INTEL);
      shader_image_atomic_int64_EXT.link(shader_image_footprint_NV);
      shader_demote_to_helper_invocation_EXT.link(shader_image_atomic_int64_EXT);
      shader_clock_KHR.link(shader_demote_to_helper_invocation_EXT);
      shader_atomic_float_EXT.link(shader_clock_KHR);
      representative_fragment_test_NV.link(shader_atomic_float_EXT);
      robustness2_EXT.link(representative_fragment_test_NV);
      ray_tracing_pipeline_KHR.link(robustness2_EXT);
      ray_query_KHR.link(ray_tracing_pipeline_KHR);
      provoking_vertex_EXT.link(ray_query_KHR);
      private_data_EXT.link(provoking_vertex_EXT);
      pipeline_creation_cache_control_EXT.link(private_data_EXT);
      pipeline_executable_properties_KHR.link(pipeline_creation_cache_control_EXT);
      portability_subset_KHR.link(pipeline_executable_properties_KHR);
      performance_query_KHR.link(portability_subset_KHR);
      mutable_descriptor_type_VALVE.link(performance_query_KHR);
      mesh_shader_NV.link(mutable_descriptor_type_VALVE);
      memory_priority_EXT.link(mesh_shader_NV);
      line_rasterization_EXT.link(memory_priority_EXT);
      inherited_viewport_scissor_NV.link(line_rasterization_EXT);
      inline_uniform_block_EXT.link(inherited_viewport_scissor_NV);
      index_type_uint8_EXT.link(inline_uniform_block_EXT);
      image_robustness_EXT.link(index_type_uint8_EXT);
      fragment_shading_rate_enums_NV.link(image_robustness_EXT);
      fragment_shading_rate_KHR.link(fragment_shading_rate_enums_NV);
      fragment_shader_barycentric_NV.link(fragment_shading_rate_KHR);
      fragment_shader_interlock_EXT.link(fragment_shader_barycentric_NV);
      fragment_density_map2_EXT.link(fragment_shader_interlock_EXT);
      fragment_density_map_EXT.link(fragment_density_map2_EXT);
      extended_dynamic_state2_EXT.link(fragment_density_map_EXT);
      extended_dynamic_state_EXT.link(extended_dynamic_state2_EXT);
      exclusive_scissor_NV.link(extended_dynamic_state_EXT);
      dedicated_allocation_image_aliasing_NV.link(exclusive_scissor_NV);
      diagnostics_config_NV.link(dedicated_allocation_image_aliasing_NV);
      depth_clip_enable_EXT.link(diagnostics_config_NV);
      device_memory_report_EXT.link(depth_clip_enable_EXT);
      device_generated_commands_NV.link(device_memory_report_EXT);
      custom_border_color_EXT.link(device_generated_commands_NV);
      coverage_reduction_mode_NV.link(custom_border_color_EXT);
      corner_sampled_image_NV.link(coverage_reduction_mode_NV);
      compute_shader_derivatives_NV.link(corner_sampled_image_NV);
      coherent_memory_AMD.link(compute_shader_derivatives_NV);
      cooperative_matrix_NV.link(coherent_memory_AMD);
      conditional_rendering_EXT.link(cooperative_matrix_NV);
      color_write_enable_EXT.link(conditional_rendering_EXT);
      blend_operation_advanced_EXT.link(color_write_enable_EXT);
      acceleration_structure_KHR.link(blend_operation_advanced_EXT);
      astc_decode_EXT.link(acceleration_structure_KHR);
      x4444_formats_EXT.link(astc_decode_EXT);
      vulkan_1_2.link(x4444_formats_EXT);
      vulkan_1_1.link(vulkan_1_2);
      vulkan_1_0.link(vulkan_1_1);
    }
  };

}

#endif  //VALKYRIE_GRAPHICS_API_FEATURES_HPP
