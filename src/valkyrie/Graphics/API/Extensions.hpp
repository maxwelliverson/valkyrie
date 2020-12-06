//
// Created by Maxwell on 2020-11-25.
//

#ifndef VALKYRIE_IMPL_EXTENSIONS_HPP
#define VALKYRIE_IMPL_EXTENSIONS_HPP

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Types.hpp>

#include <string_view>

#define VK_match_extension(Vendor, Ext_) if (extName == #Ext_) return Ext::Vendor##_##Ext_;
#define VK_match_all_extensions(Vendor, ...) if (extName.starts_with(#Vendor"_")) { \
  extName.remove_prefix(sizeof(#Vendor));                                           \
  VK_foreach(VK_bind_front(VK_match_extension, Vendor), ##__VA_ARGS__)              \
  return Ext::invalid;                                                              \
}

namespace valkyrie::Graphics::API{
  enum class InstanceExtensionID : u32{
    KHR_android_surface,
    KHR_display,
    KHR_get_display_properties2,
    KHR_get_surface_capabilities2,
    KHR_surface,
    KHR_surface_protected_capabilities,
    KHR_wayland_surface,
    KHR_win32_surface,
    KHR_xcb_surface,
    KHR_xlib_surface,
    EXT_acquire_xlib_display,
    EXT_debug_utils,
    EXT_direct_mode_display,
    EXT_directfb_surface,
    EXT_display_surface_counter,
    EXT_headless_surface,
    EXT_metal_surface,
    EXT_swapchain_colorspace,
    EXT_validation_features,
    FUCHSIA_imagepipe_surface,
    GGP_stream_descriptor_surface,
    NN_vi_surface,
    invalid = std::numeric_limits<unsigned>::max()
  };
  enum class DeviceExtensionID : u32{
    KHR_acceleration_structure,
    KHR_copy_commands2,
    KHR_deferred_host_operations,
    KHR_display_swapchain,
    KHR_external_fence_fd,
    KHR_external_fence_win32,
    KHR_external_memory_fd,
    KHR_external_memory_win32,
    KHR_external_semaphore_fd,
    KHR_external_semaphore_win32,
    KHR_fragment_shading_rate,
    KHR_incremental_present,
    KHR_performance_query,
    KHR_pipeline_executable_properties,
    KHR_pipeline_library,
    KHR_push_descriptor,
    KHR_ray_query,
    KHR_ray_tracing_pipeline,
    KHR_shader_clock,
    KHR_shader_non_semantic_info,
    KHR_shader_terminate_invocation,
    KHR_shared_presentable_image,
    KHR_swapchain,
    KHR_swapchain_mutable_format,
    KHR_win32_keyed_mutex,
    EXT_4444_formats,
    EXT_astc_decode_mode,
    EXT_blend_operation_advanced,
    EXT_calibrated_timestamps,
    EXT_conditional_rendering,
    EXT_conservative_rasterization,
    EXT_custom_border_color,
    EXT_depth_clip_enable,
    EXT_depth_range_unrestricted,
    EXT_device_memory_report,
    EXT_discard_rectangles,
    EXT_display_control,
    EXT_extended_dynamic_state,
    EXT_external_memory_dma_buf,
    EXT_external_memory_host,
    EXT_filter_cubic,
    EXT_fragment_density_map,
    EXT_fragment_density_map2,
    EXT_fragment_shader_interlock,
    EXT_full_screen_exclusive,
    EXT_global_priority,
    EXT_hdr_metadata,
    EXT_image_drm_format_modifier,
    EXT_image_robustness,
    EXT_index_type_uint8,
    EXT_inline_uniform_block,
    EXT_line_rasterization,
    EXT_memory_budget,
    EXT_memory_priority,
    EXT_pci_bus_info,
    EXT_pipeline_creation_cache_control,
    EXT_pipeline_creation_feedback,
    EXT_post_depth_coverage,
    EXT_private_data,
    EXT_queue_family_foreign,
    EXT_robustness2,
    EXT_sample_locations,
    EXT_shader_atomic_float,
    EXT_shader_demote_to_helper_invocation,
    EXT_shader_image_atomic_int64,
    EXT_shader_stencil_export,
    EXT_subgroup_size_control,
    EXT_texel_buffer_alignment,
    EXT_texture_compression_astc_hdr,
    EXT_tooling_info,
    EXT_transform_feedback,
    EXT_validation_cache,
    EXT_vertex_attribute_divisor,
    EXT_ycbcr_image_arrays,
    AMD_buffer_marker,
    AMD_device_coherent_memory,
    AMD_display_native_hdr,
    AMD_gcn_shader,
    AMD_memory_overallocation_behavior,
    AMD_mixed_attachment_samples,
    AMD_pipeline_compiler_control,
    AMD_rasterization_order,
    AMD_shader_ballot,
    AMD_shader_core_properties,
    AMD_shader_core_properties2,
    AMD_shader_explicit_vertex_parameter,
    AMD_shader_fragment_mask,
    AMD_shader_image_load_store_lod,
    AMD_shader_info,
    AMD_shader_trinary_minmax,
    AMD_texture_gather_bias_lod,
    ANDROID_external_memory_android_hardware_buffer,
    GGP_frame_token,
    GOOGLE_decorate_string,
    GOOGLE_display_timing,
    GOOGLE_hlsl_functionality1,
    GOOGLE_user_type,
    IMG_filter_cubic,
    IMG_format_pvrtc,
    INTEL_performance_query,
    INTEL_shader_integer_functions2,
    NV_clip_space_w_scaling,
    NV_compute_shader_derivatives,
    NV_cooperative_matrix,
    NV_corner_sampled_image,
    NV_coverage_reduction_mode,
    NV_dedicated_allocation_image_aliasing,
    NV_device_diagnostic_checkpoints,
    NV_device_diagnostics_config,
    NV_device_generated_commands,
    NV_fill_rectangle,
    NV_fragment_coverage_to_color,
    NV_fragment_shader_barycentric,
    NV_fragment_shading_rate_enums,
    NV_framebuffer_mixed_samples,
    NV_geometry_shader_passthrough,
    NV_mesh_shader,
    NV_ray_tracing,
    NV_representative_fragment_test,
    NV_sample_mask_override_coverage,
    NV_scissor_exclusive,
    NV_shader_image_footprint,
    NV_shader_sm_builtins,
    NV_shader_subgroup_partitioned,
    NV_shading_rate_image,
    NV_viewport_array2,
    NV_viewport_swizzle,
    NVX_image_view_handle,
    NVX_multiview_per_view_attributes,
    QCOM_render_pass_shader_resolve,
    QCOM_render_pass_store_ops,
    QCOM_render_pass_transform,
    QCOM_rotated_copy_commands,
    invalid = std::numeric_limits<unsigned>::max()
  };
  enum class LayerID : u32{
    KHRONOS_validation,
    LUNARG_api_dump,
    LUNARG_device_simulation,
    LUNARG_gfxreconstruct,
    LUNARG_monitor,
    LUNARG_screenshot,
    NV_nomad,
    NV_optimus,
    VALVE_steam_overlay,
    invalid = std::numeric_limits<unsigned>::max()
  };

  inline constexpr InstanceExtensionID getInstanceExtensionID(std::string_view extName) noexcept {
    if (extName.starts_with(""))
      extName.remove_prefix(3);
    switch(extName.front()) {
      case 'E':
        if (extName.starts_with("EXT_")) {
          extName.remove_prefix(4);
          switch (extName.front()) {
            case 'a':
              if (extName == "acquire_xlib_display")
                return InstanceExtensionID::EXT_acquire_xlib_display;
              break;
            case 'd':
              extName.remove_prefix(1);
              switch (extName.front()) {
                case 'e':
                  if (extName == "ebug_utils")
                    return InstanceExtensionID::EXT_debug_utils;
                  break;
                case 'i':
                  extName.remove_prefix(1);
                  switch (extName.front()) {
                    case 'r':
                      if (extName == "rect_mode_display")
                        return InstanceExtensionID::EXT_direct_mode_display;
                      if (extName == "rectfb_surface")
                        return InstanceExtensionID::EXT_directfb_surface;
                      break;
                    case 's':
                      if (extName == "splay_surface_counter")
                        return InstanceExtensionID::EXT_display_surface_counter;
                  }
              } break;
            case 'h':
              if (extName == "headless_surface")
                return InstanceExtensionID::EXT_headless_surface;
              break;
            case 'm':
              if (extName == "metal_surface")
                return InstanceExtensionID::EXT_metal_surface;
              break;
            case 's':
              if (extName == "swapchain_colorspace")
                return InstanceExtensionID::EXT_swapchain_colorspace;
              break;
            case 'v':
              if (extName == "validation_features")
                return InstanceExtensionID::EXT_validation_features;
          }
        } break;
      case 'F':
        if (extName == "FUCHSIA_imagepipe_surface")
          return InstanceExtensionID::FUCHSIA_imagepipe_surface;
        break;
      case 'G':
        if (extName == "GGP_stream_descriptor_surface")
          return InstanceExtensionID::GGP_stream_descriptor_surface;
        break;
      case 'K':
        if (extName.starts_with("KHR_")) {
          extName.remove_prefix(4);
          switch (extName.front()) {
            case 'a':
              if (extName == "android_surface")
                return InstanceExtensionID::KHR_android_surface;
              break;
            case 'd':
              if (extName == "display")
                return InstanceExtensionID::KHR_display;
              break;
            case 'g':
              if (extName.starts_with("get_")) {
                extName.remove_prefix(4);
                char front = extName.front();
                if (front == 'd' && extName == "display_properties2")
                  return InstanceExtensionID::KHR_get_display_properties2;
                if (front == 's' && extName == "surface_capabilities2")
                  return InstanceExtensionID::KHR_get_surface_capabilities2;
              }
              break;
            case 's':
              if (extName.starts_with("surface")) {
                extName.remove_prefix(7);
                if (extName.empty())
                  return InstanceExtensionID::KHR_surface;
                if (extName == "_protected_capabilities")
                  return InstanceExtensionID::KHR_surface_protected_capabilities;
              } break;
            case 'w':
              if (extName == "win32_surface")
                return InstanceExtensionID::KHR_win32_surface;
              if (extName == "wayland_surface")
                return InstanceExtensionID::KHR_wayland_surface;
              break;
            case 'x':
              if (extName == "xcb_surface")
                return InstanceExtensionID::KHR_xcb_surface;
              if (extName == "xlib_surface")
                return InstanceExtensionID::KHR_xlib_surface;
          }
        }
        break;
      case 'N':
        if (extName == "NN_vi_surface")
          return InstanceExtensionID::NN_vi_surface;
    }
    return InstanceExtensionID::invalid;
  }
  inline constexpr DeviceExtensionID   getDeviceExtensionID(std::string_view extName) noexcept {
    using Ext = DeviceExtensionID;

    if (extName.starts_with("VK_"))
      extName.remove_prefix(3);

    VK_match_all_extensions(KHR,
                            acceleration_structure,
                            copy_commands2,
                            deferred_host_operations,
                            display_swapchain,
                            external_fence_fd,
                            external_fence_win32,
                            external_memory_fd,
                            external_memory_win32,
                            external_semaphore_fd,
                            external_semaphore_win32,
                            fragment_shading_rate,
                            incremental_present,
                            performance_query,
                            pipeline_executable_properties,
                            pipeline_library,
                            push_descriptor,
                            ray_query,
                            ray_tracing_pipeline,
                            shader_clock,
                            shader_non_semantic_info,
                            shader_terminate_invocation,
                            shared_presentable_image,
                            swapchain,
                            swapchain_mutable_format,
                            win32_keyed_mutex)

    VK_match_all_extensions(EXT,
                            4444_formats,
                            astc_decode_mode,
                            blend_operation_advanced,
                            calibrated_timestamps,
                            conditional_rendering,
                            conservative_rasterization,
                            custom_border_color,
                            depth_clip_enable,
                            depth_range_unrestricted,
                            device_memory_report,
                            discard_rectangles,
                            display_control,
                            extended_dynamic_state,
                            external_memory_dma_buf,
                            external_memory_host,
                            filter_cubic,
                            fragment_density_map,
                            fragment_density_map2,
                            fragment_shader_interlock,
                            full_screen_exclusive,
                            global_priority,
                            hdr_metadata,
                            image_drm_format_modifier,
                            image_robustness,
                            index_type_uint8,
                            inline_uniform_block,
                            line_rasterization,
                            memory_budget,
                            memory_priority,
                            pci_bus_info,
                            pipeline_creation_cache_control,
                            pipeline_creation_feedback,
                            post_depth_coverage,
                            private_data,
                            queue_family_foreign,
                            robustness2,
                            sample_locations,
                            shader_atomic_float,
                            shader_demote_to_helper_invocation,
                            shader_image_atomic_int64,
                            shader_stencil_export,
                            subgroup_size_control,
                            texel_buffer_alignment,
                            texture_compression_astc_hdr,
                            tooling_info,
                            transform_feedback,
                            validation_cache,
                            vertex_attribute_divisor,
                            ycbcr_image_arrays)

    VK_match_all_extensions(AMD,
                            buffer_marker,
                            device_coherent_memory,
                            display_native_hdr,
                            gcn_shader,
                            memory_overallocation_behavior,
                            mixed_attachment_samples,
                            pipeline_compiler_control,
                            rasterization_order,
                            shader_ballot,
                            shader_core_properties,
                            shader_core_properties2,
                            shader_explicit_vertex_parameter,
                            shader_fragment_mask,
                            shader_image_load_store_lod,
                            shader_info,
                            shader_trinary_minmax,
                            texture_gather_bias_lod)
    VK_match_all_extensions(NVX,
                            image_view_handle,
                            multiview_per_view_attributes)

    VK_match_all_extensions(NV, clip_space_w_scaling,
                            compute_shader_derivatives,
                            cooperative_matrix,
                            corner_sampled_image,
                            coverage_reduction_mode,
                            dedicated_allocation_image_aliasing,
                            device_diagnostic_checkpoints,
                            device_diagnostics_config,
                            device_generated_commands,
                            fill_rectangle,
                            fragment_coverage_to_color,
                            fragment_shader_barycentric,
                            fragment_shading_rate_enums,
                            framebuffer_mixed_samples,
                            geometry_shader_passthrough,
                            mesh_shader,
                            ray_tracing,
                            representative_fragment_test,
                            sample_mask_override_coverage,
                            scissor_exclusive,
                            shader_image_footprint,
                            shader_sm_builtins,
                            shader_subgroup_partitioned,
                            shading_rate_image,
                            viewport_array2,
                            viewport_swizzle)

    VK_match_all_extensions(QCOM,
                            render_pass_shader_resolve,
                            render_pass_store_ops,
                            render_pass_transform,
                            rotated_copy_commands)

    VK_match_all_extensions(GOOGLE,
                            decorate_string,
                            display_timing,
                            hlsl_functionality1,
                            user_type)

    VK_match_all_extensions(IMG, filter_cubic, format_pvrtc)
    VK_match_all_extensions(INTEL, performance_query, shader_integer_functions2)

    VK_match_extension(GGP, frame_token)
    VK_match_extension(ANDROID, external_memory_android_hardware_buffer)
  }
  inline constexpr LayerID             getLayerID(std::string_view extName) noexcept {
    using Ext = LayerID;

    if (extName.starts_with("VK_LAYER_"))
      extName.remove_prefix(9);

    VK_match_extension(KHRONOS, validation);

    VK_match_all_extensions(LUNARG,
                            api_dump,
                            device_simulation,
                            gfxreconstruct,
                            monitor,
                            screenshot);

    VK_match_all_extensions(NV, nomad, optimus);

    VK_match_extension(VALVE, steam_overlay);

    return Ext::invalid;
  }
}

#endif//VALKYRIE_IMPL_EXTENSIONS_HPP
