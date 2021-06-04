//
// Created by Maxwell on 2020-12-02.
//

#ifndef VALKYRIE_VULKAN_ENUMS_HPP
#define VALKYRIE_VULKAN_ENUMS_HPP

#include <valkyrie/utility/bitflag.hpp>
#include <valkyrie/adt/string_view.hpp>

namespace valkyrie::graphics::api {

  enum class result : i32{
    success = 0,
    not_ready = 1,
    timeout = 2,
    event_set = 3,
    event_reset = 4,
    incomplete = 5,
    error_out_of_host_memory = -1,
    error_out_of_device_memory = -2,
    error_initialization_failed = -3,
    error_device_lost = -4,
    error_memory_map_failed = -5,
    error_layer_not_present = -6,
    error_extension_not_present = -7,
    error_feature_not_present = -8,
    error_incompatible_driver = -9,
    error_too_many_objects = -10,
    error_format_not_supported = -11,
    error_fragmented_pool = -12,
    error_unknown = -13,
    error_out_of_pool_memory = -1000069000,
    error_invalid_external_handle = -1000072003,
    error_fragmentation = -1000161000,
    error_invalid_opaque_capture_address = -1000257000,
    error_surface_lost = -1000000000,
    error_native_window_in_use = -1000000001,
    suboptimal = 1000001003,
    error_out_of_date = -1000001004,
    error_incompatible_display = -1000003001,
    error_validation_failed = -1000011001,
    error_invalid_shader = -1000012000,
    error_invalid_drm_format_modifier_plane_layout = -1000158000,
    error_not_permitted = -1000174001,
    error_full_screen_exclusive_mode_lost = -1000255000,
    thread_idle = 1000268000,
    thread_done = 1000268001,
    operation_deferred = 1000268002,
    operation_not_deferred = 1000268003,
    pipeline_compile_required = 1000297000
  };
  enum class structure_type : u32 {
    application_info = 0,
    instance_create_info = 1,
    device_queue_create_info = 2,
    device_create_info = 3,
    submit_info = 4,
    memory_allocate_info = 5,
    mapped_memory_range = 6,
    bind_sparse_info = 7,
    fence_create_info = 8,
    semaphore_create_info = 9,
    event_create_info = 10,
    query_pool_create_info = 11,
    buffer_create_info = 12,
    buffer_view_create_info = 13,
    image_create_info = 14,
    image_view_create_info = 15,
    shader_module_create_info = 16,
    pipeline_cache_create_info = 17,
    pipeline_shader_stage_create_info = 18,
    pipeline_vertex_input_state_create_info = 19,
    pipeline_input_assembly_state_create_info = 20,
    pipeline_tessellation_state_create_info = 21,
    pipeline_viewport_state_create_info = 22,
    pipeline_rasterization_state_create_info = 23,
    pipeline_multisample_state_create_info = 24,
    pipeline_depth_stencil_state_create_info = 25,
    pipeline_color_blend_state_create_info = 26,
    pipeline_dynamic_state_create_info = 27,
    graphics_pipeline_create_info = 28,
    compute_pipeline_create_info = 29,
    pipeline_layout_create_info = 30,
    sampler_create_info = 31,
    descriptor_set_layout_create_info = 32,
    descriptor_pool_create_info = 33,
    descriptor_set_allocate_info = 34,
    write_descriptor_set = 35,
    copy_descriptor_set = 36,
    framebuffer_create_info = 37,
    render_pass_create_info = 38,
    command_pool_create_info = 39,
    command_buffer_allocate_info = 40,
    command_buffer_inheritance_info = 41,
    command_buffer_begin_info = 42,
    render_pass_begin_info = 43,
    buffer_memory_barrier = 44,
    image_memory_barrier = 45,
    memory_barrier = 46,
    loader_instance_create_info = 47,
    loader_device_create_info = 48,
    physical_device_subgroup_properties = 1000094000,
    bind_buffer_memory_info = 1000157000,
    bind_image_memory_info = 1000157001,
    physical_device_16bit_storage_features = 1000083000,
    memory_dedicated_requirements = 1000127000,
    memory_dedicated_allocate_info = 1000127001,
    memory_allocate_flags_info = 1000060000,
    device_group_render_pass_begin_info = 1000060003,
    device_group_command_buffer_begin_info = 1000060004,
    device_group_submit_info = 1000060005,
    device_group_bind_sparse_info = 1000060006,
    bind_buffer_memory_device_group_info = 1000060013,
    bind_image_memory_device_group_info = 1000060014,
    physical_device_group_properties = 1000070000,
    device_group_device_create_info = 1000070001,
    buffer_memory_requirements_info2 = 1000146000,
    image_memory_requirements_info2 = 1000146001,
    image_sparse_memory_requirements_info2 = 1000146002,
    memory_requirements2 = 1000146003,
    sparse_image_memory_requirements2 = 1000146004,
    physical_device_features2 = 1000059000,
    physical_device_properties2 = 1000059001,
    format_properties2 = 1000059002,
    image_format_properties2 = 1000059003,
    physical_device_image_format_info2 = 1000059004,
    queue_family_properties2 = 1000059005,
    physical_device_memory_properties2 = 1000059006,
    sparse_image_format_properties2 = 1000059007,
    physical_device_sparse_image_format_info2 = 1000059008,
    physical_device_point_clipping_properties = 1000117000,
    render_pass_input_attachment_aspect_create_info = 1000117001,
    image_view_usage_create_info = 1000117002,
    pipeline_tessellation_domain_origin_state_create_info = 1000117003,
    render_pass_multiview_create_info = 1000053000,
    physical_device_multiview_features = 1000053001,
    physical_device_multiview_properties = 1000053002,
    physical_device_variable_pointers_features = 1000120000,
    protected_submit_info = 1000145000,
    physical_device_protected_memory_features = 1000145001,
    physical_device_protected_memory_properties = 1000145002,
    device_queue_info2 = 1000145003,
    sampler_ycbcr_conversion_create_info = 1000156000,
    sampler_ycbcr_conversion_info = 1000156001,
    bind_image_plane_memory_info = 1000156002,
    image_plane_memory_requirements_info = 1000156003,
    physical_device_sampler_ycbcr_conversion_features = 1000156004,
    sampler_ycbcr_conversion_image_format_properties = 1000156005,
    descriptor_update_template_create_info = 1000085000,
    physical_device_external_image_format_info = 1000071000,
    external_image_format_properties = 1000071001,
    physical_device_external_buffer_info = 1000071002,
    external_buffer_properties = 1000071003,
    physical_device_id_properties = 1000071004,
    external_memory_buffer_create_info = 1000072000,
    external_memory_image_create_info = 1000072001,
    export_memory_allocate_info = 1000072002,
    physical_device_external_fence_info = 1000112000,
    external_fence_properties = 1000112001,
    export_fence_create_info = 1000113000,
    export_semaphore_create_info = 1000077000,
    physical_device_external_semaphore_info = 1000076000,
    external_semaphore_properties = 1000076001,
    physical_device_maintenance3_properties = 1000168000,
    descriptor_set_layout_support = 1000168001,
    physical_device_shader_draw_parameters_features = 1000063000,
    physical_device_vulkan_1_1_features = 49,
    physical_device_vulkan_1_1_properties = 50,
    physical_device_vulkan_1_2_features = 51,
    physical_device_vulkan_1_2_properties = 52,
    image_format_list_create_info = 1000147000,
    attachment_description2 = 1000109000,
    attachment_reference2 = 1000109001,
    subpass_description2 = 1000109002,
    subpass_dependency2 = 1000109003,
    render_pass_create_info2 = 1000109004,
    subpass_begin_info = 1000109005,
    subpass_end_info = 1000109006,
    physical_device_8bit_storage_features = 1000177000,
    physical_device_driver_properties = 1000196000,
    physical_device_shader_atomic_int64_features = 1000180000,
    physical_device_shader_float16_int8_features = 1000082000,
    physical_device_float_controls_properties = 1000197000,
    descriptor_set_layout_binding_flags_create_info = 1000161000,
    physical_device_descriptor_indexing_features = 1000161001,
    physical_device_descriptor_indexing_properties = 1000161002,
    descriptor_set_variable_descriptor_count_allocate_info = 1000161003,
    descriptor_set_variable_descriptor_count_layout_support = 1000161004,
    physical_device_depth_stencil_resolve_properties = 1000199000,
    subpass_description_depth_stencil_resolve = 1000199001,
    physical_device_scalar_block_layout_features = 1000221000,
    image_stencil_usage_create_info = 1000246000,
    physical_device_sampler_filter_minmax_properties = 1000130000,
    sampler_reduction_mode_create_info = 1000130001,
    physical_device_vulkan_memory_model_features = 1000211000,
    physical_device_imageless_framebuffer_features = 1000108000,
    framebuffer_attachments_create_info = 1000108001,
    framebuffer_attachment_image_info = 1000108002,
    render_pass_attachment_begin_info = 1000108003,
    physical_device_uniform_buffer_standard_layout_features = 1000253000,
    physical_device_shader_subgroup_extended_types_features = 1000175000,
    physical_device_separate_depth_stencil_layouts_features = 1000241000,
    attachment_reference_stencil_layout = 1000241001,
    attachment_description_stencil_layout = 1000241002,
    physical_device_host_query_reset_features = 1000261000,
    physical_device_timeline_semaphore_features = 1000207000,
    physical_device_timeline_semaphore_properties = 1000207001,
    semaphore_type_create_info = 1000207002,
    timeline_semaphore_submit_info = 1000207003,
    semaphore_wait_info = 1000207004,
    semaphore_signal_info = 1000207005,
    physical_device_buffer_device_address_features = 1000257000,
    buffer_device_address_info = 1000244001,
    buffer_opaque_capture_address_create_info = 1000257002,
    memory_opaque_capture_address_allocate_info = 1000257003,
    device_memory_opaque_capture_address_info = 1000257004,
    swapchain_create_info_KHR = 1000001000,
    present_info_KHR = 1000001001,
    device_group_present_capabilities_KHR = 1000060007,
    image_swapchain_create_info_KHR = 1000060008,
    bind_image_memory_swapchain_info_KHR = 1000060009,
    acquire_next_image_info_KHR = 1000060010,
    device_group_present_info_KHR = 1000060011,
    device_group_swapchain_create_info_KHR = 1000060012,
    display_mode_create_info_KHR = 1000002000,
    display_surface_create_info_KHR = 1000002001,
    display_present_info_KHR = 1000003000,
    xlib_surface_create_info_KHR = 1000004000,
    xcb_surface_create_info_KHR = 1000005000,
    wayland_surface_create_info_KHR = 1000006000,
    android_surface_create_info_KHR = 1000008000,
    win32_surface_create_info_KHR = 1000009000,
    debug_report_callback_create_info_EXT = 1000011000,
    pipeline_rasterization_state_rasterization_order_AMD = 1000018000,
    debug_marker_object_name_info_EXT = 1000022000,
    debug_marker_object_tag_info_EXT = 1000022001,
    debug_marker_marker_info_EXT = 1000022002,
    video_profile_KHR = 1000023000,
    video_capabilities_KHR = 1000023001,
    video_picture_resource_KHR = 1000023002,
    video_get_memory_properties_KHR = 1000023003,
    video_bind_memory_KHR = 1000023004,
    video_session_create_info_KHR = 1000023005,
    video_session_parameters_create_info_KHR = 1000023006,
    video_session_parameters_update_info_KHR = 1000023007,
    video_begin_coding_info_KHR = 1000023008,
    video_end_coding_info_KHR = 1000023009,
    video_coding_control_info_KHR = 1000023010,
    video_reference_slot_KHR = 1000023011,
    video_queue_family_properties2_KHR = 1000023012,
    video_profiles_KHR = 1000023013,
    physical_device_video_format_info_KHR = 1000023014,
    video_format_properties_KHR = 1000023015,
    video_decode_info_KHR = 1000024000,
    video_encode_info_KHR = 1000299000,
    video_encode_rate_control_info_KHR = 1000299001,
    dedicated_allocation_image_create_info_NV = 1000026000,
    dedicated_allocation_buffer_create_info_NV = 1000026001,
    dedicated_allocation_memory_allocate_info_NV = 1000026002,
    physical_device_transform_feedback_features_EXT = 1000028000,
    physical_device_transform_feedback_properties_EXT = 1000028001,
    pipeline_rasterization_state_stream_create_info_EXT = 1000028002,
    image_view_handle_info_NVX = 1000030000,
    image_view_address_properties_NVX = 1000030001,
    video_encode_h264_capabilities_EXT = 1000038000,
    video_encode_h264_session_create_info_EXT = 1000038001,
    video_encode_h264_session_parameters_create_info_EXT = 1000038002,
    video_encode_h264_session_parameters_add_info_EXT = 1000038003,
    video_encode_h264_vcl_frame_info_EXT = 1000038004,
    video_encode_h264_dpb_slot_info_EXT = 1000038005,
    video_encode_h264_nalu_slice_EXT = 1000038006,
    video_encode_h264_emit_picture_parameters_EXT = 1000038007,
    video_encode_h264_profile_EXT = 1000038008,
    video_decode_h264_capabilities_EXT = 1000040000,
    video_decode_h264_session_create_info_EXT = 1000040001,
    video_decode_h264_picture_info_EXT = 1000040002,
    video_decode_h264_mvc_EXT = 1000040003,
    video_decode_h264_profile_EXT = 1000040004,
    video_decode_h264_session_parameters_create_info_EXT = 1000040005,
    video_decode_h264_session_parameters_add_info_EXT = 1000040006,
    video_decode_h264_dpb_slot_info_EXT = 1000040007,
    texture_lod_gather_format_properties_AMD = 1000041000,
    stream_descriptor_surface_create_info_GGP = 1000049000,
    physical_device_corner_sampled_image_features_NV = 1000050000,
    external_memory_image_create_info_NV = 1000056000,
    export_memory_allocate_info_NV = 1000056001,
    import_memory_win32_handle_info_NV = 1000057000,
    export_memory_win32_handle_info_NV = 1000057001,
    win32_keyed_mutex_acquire_release_info_NV = 1000058000,
    validation_flags_EXT = 1000061000,
    vi_surface_create_info_NN = 1000062000,
    physical_device_texture_compression_astc_hdr_features_EXT = 1000066000,
    image_view_astc_decode_mode_EXT = 1000067000,
    physical_device_astc_decode_features_EXT = 1000067001,
    import_memory_win32_handle_info_KHR = 1000073000,
    export_memory_win32_handle_info_KHR = 1000073001,
    memory_win32_handle_properties_KHR = 1000073002,
    memory_get_win32_handle_info_KHR = 1000073003,
    import_memory_fd_info_KHR = 1000074000,
    memory_fd_properties_KHR = 1000074001,
    memory_get_fd_info_KHR = 1000074002,
    win32_keyed_mutex_acquire_release_info_KHR = 1000075000,
    import_semaphore_win32_handle_info_KHR = 1000078000,
    export_semaphore_win32_handle_info_KHR = 1000078001,
    d3d12_fence_submit_info_KHR = 1000078002,
    semaphore_get_win32_handle_info_KHR = 1000078003,
    import_semaphore_fd_info_KHR = 1000079000,
    semaphore_get_fd_info_KHR = 1000079001,
    physical_device_push_descriptor_properties_KHR = 1000080000,
    command_buffer_inheritance_conditional_rendering_info_EXT = 1000081000,
    physical_device_conditional_rendering_features_EXT = 1000081001,
    conditional_rendering_begin_info_EXT = 1000081002,
    present_regions_KHR = 1000084000,
    pipeline_viewport_w_scaling_state_create_info_NV = 1000087000,
    surface_capabilities2_EXT = 1000090000,
    display_power_info_EXT = 1000091000,
    device_event_info_EXT = 1000091001,
    display_event_info_EXT = 1000091002,
    swapchain_counter_create_info_EXT = 1000091003,
    present_times_info_GOOGLE = 1000092000,
    physical_device_multiview_per_view_attributes_properties_NVX = 1000097000,
    pipeline_viewport_swizzle_state_create_info_NV = 1000098000,
    physical_device_discard_rectangle_properties_EXT = 1000099000,
    pipeline_discard_rectangle_state_create_info_EXT = 1000099001,
    physical_device_conservative_rasterization_properties_EXT = 1000101000,
    pipeline_rasterization_conservative_state_create_info_EXT = 1000101001,
    physical_device_depth_clip_enable_features_EXT = 1000102000,
    pipeline_rasterization_depth_clip_state_create_info_EXT = 1000102001,
    hdr_metadata_EXT = 1000105000,
    shared_present_surface_capabilities_KHR = 1000111000,
    import_fence_win32_handle_info_KHR = 1000114000,
    export_fence_win32_handle_info_KHR = 1000114001,
    fence_get_win32_handle_info_KHR = 1000114002,
    import_fence_fd_info_KHR = 1000115000,
    fence_get_fd_info_KHR = 1000115001,
    physical_device_performance_query_features_KHR = 1000116000,
    physical_device_performance_query_properties_KHR = 1000116001,
    query_pool_performance_create_info_KHR = 1000116002,
    performance_query_submit_info_KHR = 1000116003,
    acquire_profiling_lock_info_KHR = 1000116004,
    performance_counter_KHR = 1000116005,
    performance_counter_description_KHR = 1000116006,
    physical_device_surface_info2_KHR = 1000119000,
    surface_capabilities2_KHR = 1000119001,
    surface_format2_KHR = 1000119002,
    display_properties2_KHR = 1000121000,
    display_plane_properties2_KHR = 1000121001,
    display_mode_properties2_KHR = 1000121002,
    display_plane_info2_KHR = 1000121003,
    display_plane_capabilities2_KHR = 1000121004,
    ios_surface_create_info_MVK = 1000122000,
    macos_surface_create_info_MVK = 1000123000,
    debug_utils_object_name_info_EXT = 1000128000,
    debug_utils_object_tag_info_EXT = 1000128001,
    debug_utils_label_EXT = 1000128002,
    debug_utils_messenger_callback_data_EXT = 1000128003,
    debug_utils_messenger_create_info_EXT = 1000128004,
    android_hardware_buffer_usage_ANDROID = 1000129000,
    android_hardware_buffer_properties_ANDROID = 1000129001,
    android_hardware_buffer_format_properties_ANDROID = 1000129002,
    import_android_hardware_buffer_info_ANDROID = 1000129003,
    memory_get_android_hardware_buffer_info_ANDROID = 1000129004,
    external_format_ANDROID = 1000129005,
    physical_device_inline_uniform_block_features_EXT = 1000138000,
    physical_device_inline_uniform_block_properties_EXT = 1000138001,
    write_descriptor_set_inline_uniform_block_EXT = 1000138002,
    descriptor_pool_inline_uniform_block_create_info_EXT = 1000138003,
    sample_locations_info_EXT = 1000143000,
    render_pass_sample_locations_begin_info_EXT = 1000143001,
    pipeline_sample_locations_state_create_info_EXT = 1000143002,
    physical_device_sample_locations_properties_EXT = 1000143003,
    multisample_properties_EXT = 1000143004,
    physical_device_blend_operation_advanced_features_EXT = 1000148000,
    physical_device_blend_operation_advanced_properties_EXT = 1000148001,
    pipeline_color_blend_advanced_state_create_info_EXT = 1000148002,
    pipeline_coverage_to_color_state_create_info_NV = 1000149000,
    write_descriptor_set_acceleration_structure_KHR = 1000150007,
    acceleration_structure_build_geometry_info_KHR = 1000150000,
    acceleration_structure_device_address_info_KHR = 1000150002,
    acceleration_structure_geometry_aabbs_data_KHR = 1000150003,
    acceleration_structure_geometry_instances_data_KHR = 1000150004,
    acceleration_structure_geometry_triangles_data_KHR = 1000150005,
    acceleration_structure_geometry_KHR = 1000150006,
    acceleration_structure_version_info_KHR = 1000150009,
    copy_acceleration_structure_info_KHR = 1000150010,
    copy_acceleration_structure_to_memory_info_KHR = 1000150011,
    copy_memory_to_acceleration_structure_info_KHR = 1000150012,
    physical_device_acceleration_structure_features_KHR = 1000150013,
    physical_device_acceleration_structure_properties_KHR = 1000150014,
    acceleration_structure_create_info_KHR = 1000150017,
    acceleration_structure_build_sizes_info_KHR = 1000150020,
    physical_device_ray_tracing_pipeline_features_KHR = 1000347000,
    physical_device_ray_tracing_pipeline_properties_KHR = 1000347001,
    ray_tracing_pipeline_create_info_KHR = 1000150015,
    ray_tracing_shader_group_create_info_KHR = 1000150016,
    ray_tracing_pipeline_interface_create_info_KHR = 1000150018,
    physical_device_ray_query_features_KHR = 1000348013,
    pipeline_coverage_modulation_state_create_info_NV = 1000152000,
    physical_device_shader_sm_builtins_features_NV = 1000154000,
    physical_device_shader_sm_builtins_properties_NV = 1000154001,
    drm_format_modifier_properties_list_EXT = 1000158000,
    physical_device_image_drm_format_modifier_info_EXT = 1000158002,
    image_drm_format_modifier_list_create_info_EXT = 1000158003,
    image_drm_format_modifier_explicit_create_info_EXT = 1000158004,
    image_drm_format_modifier_properties_EXT = 1000158005,
    validation_cache_create_info_EXT = 1000160000,
    shader_module_validation_cache_create_info_EXT = 1000160001,
    physical_device_portability_subset_features_KHR = 1000163000,
    physical_device_portability_subset_properties_KHR = 1000163001,
    pipeline_viewport_shading_rate_image_state_create_info_NV = 1000164000,
    physical_device_shading_rate_image_features_NV = 1000164001,
    physical_device_shading_rate_image_properties_NV = 1000164002,
    pipeline_viewport_coarse_sample_order_state_create_info_NV = 1000164005,
    ray_tracing_pipeline_create_info_NV = 1000165000,
    acceleration_structure_create_info_NV = 1000165001,
    geometry_NV = 1000165003,
    geometry_triangles_NV = 1000165004,
    geometry_aabb_NV = 1000165005,
    bind_acceleration_structure_memory_info_NV = 1000165006,
    write_descriptor_set_acceleration_structure_NV = 1000165007,
    acceleration_structure_memory_requirements_info_NV = 1000165008,
    physical_device_ray_tracing_properties_NV = 1000165009,
    ray_tracing_shader_group_create_info_NV = 1000165011,
    acceleration_structure_info_NV = 1000165012,
    physical_device_representative_fragment_test_features_NV = 1000166000,
    pipeline_representative_fragment_test_state_create_info_NV = 1000166001,
    physical_device_image_view_image_format_info_EXT = 1000170000,
    filter_cubic_image_view_image_format_properties_EXT = 1000170001,
    device_queue_global_priority_create_info_EXT = 1000174000,
    import_memory_host_pointer_info_EXT = 1000178000,
    memory_host_pointer_properties_EXT = 1000178001,
    physical_device_external_memory_host_properties_EXT = 1000178002,
    physical_device_shader_clock_features_KHR = 1000181000,
    pipeline_compiler_control_create_info_AMD = 1000183000,
    calibrated_timestamp_info_EXT = 1000184000,
    physical_device_shader_core_properties_AMD = 1000185000,
    video_decode_h265_capabilities_EXT = 1000187000,
    video_decode_h265_session_create_info_EXT = 1000187001,
    video_decode_h265_session_parameters_create_info_EXT = 1000187002,
    video_decode_h265_session_parameters_add_info_EXT = 1000187003,
    video_decode_h265_profile_EXT = 1000187004,
    video_decode_h265_picture_info_EXT = 1000187005,
    video_decode_h265_dpb_slot_info_EXT = 1000187006,
    device_memory_overallocation_create_info_AMD = 1000189000,
    physical_device_vertex_attribute_divisor_properties_EXT = 1000190000,
    pipeline_vertex_input_divisor_state_create_info_EXT = 1000190001,
    physical_device_vertex_attribute_divisor_features_EXT = 1000190002,
    present_frame_token_GGP = 1000191000,
    pipeline_creation_feedback_create_info_EXT = 1000192000,
    physical_device_compute_shader_derivatives_features_NV = 1000201000,
    physical_device_mesh_shader_features_NV = 1000202000,
    physical_device_mesh_shader_properties_NV = 1000202001,
    physical_device_fragment_shader_barycentric_features_NV = 1000203000,
    physical_device_shader_image_footprint_features_NV = 1000204000,
    pipeline_viewport_exclusive_scissor_state_create_info_NV = 1000205000,
    physical_device_exclusive_scissor_features_NV = 1000205002,
    checkpoint_data_NV = 1000206000,
    queue_family_checkpoint_properties_NV = 1000206001,
    physical_device_shader_integer_functions2_features_INTEL = 1000209000,
    query_pool_performance_query_create_info_INTEL = 1000210000,
    initialize_performance_api_info_INTEL = 1000210001,
    performance_marker_info_INTEL = 1000210002,
    performance_stream_marker_info_INTEL = 1000210003,
    performance_override_info_INTEL = 1000210004,
    performance_configuration_acquire_info_INTEL = 1000210005,
    physical_device_pci_bus_info_properties_EXT = 1000212000,
    display_native_hdr_surface_capabilities_AMD = 1000213000,
    swapchain_display_native_hdr_create_info_AMD = 1000213001,
    imagepipe_surface_create_info_FUCHSIA = 1000214000,
    physical_device_shader_terminate_invocation_features_KHR = 1000215000,
    metal_surface_create_info_EXT = 1000217000,
    physical_device_fragment_density_map_features_EXT = 1000218000,
    physical_device_fragment_density_map_properties_EXT = 1000218001,
    render_pass_fragment_density_map_create_info_EXT = 1000218002,
    physical_device_subgroup_size_control_properties_EXT = 1000225000,
    pipeline_shader_stage_required_subgroup_size_create_info_EXT = 1000225001,
    physical_device_subgroup_size_control_features_EXT = 1000225002,
    fragment_shading_rate_attachment_info_KHR = 1000226000,
    pipeline_fragment_shading_rate_state_create_info_KHR = 1000226001,
    physical_device_fragment_shading_rate_properties_KHR = 1000226002,
    physical_device_fragment_shading_rate_features_KHR = 1000226003,
    physical_device_fragment_shading_rate_KHR = 1000226004,
    physical_device_shader_core_properties2_AMD = 1000227000,
    physical_device_coherent_memory_features_AMD = 1000229000,
    physical_device_shader_image_atomic_int64_features_EXT = 1000234000,
    physical_device_memory_budget_properties_EXT = 1000237000,
    physical_device_memory_priority_features_EXT = 1000238000,
    memory_priority_allocate_info_EXT = 1000238001,
    surface_protected_capabilities_KHR = 1000239000,
    physical_device_dedicated_allocation_image_aliasing_features_NV = 1000240000,
    physical_device_buffer_device_address_features_EXT = 1000244000,
    buffer_device_address_create_info_EXT = 1000244002,
    physical_device_tool_properties_EXT = 1000245000,
    validation_features_EXT = 1000247000,
    physical_device_cooperative_matrix_features_NV = 1000249000,
    cooperative_matrix_properties_NV = 1000249001,
    physical_device_cooperative_matrix_properties_NV = 1000249002,
    physical_device_coverage_reduction_mode_features_NV = 1000250000,
    pipeline_coverage_reduction_state_create_info_NV = 1000250001,
    framebuffer_mixed_samples_combination_NV = 1000250002,
    physical_device_fragment_shader_interlock_features_EXT = 1000251000,
    physical_device_ycbcr_image_arrays_features_EXT = 1000252000,
    physical_device_provoking_vertex_features_EXT = 1000254000,
    pipeline_rasterization_provoking_vertex_state_create_info_EXT = 1000254001,
    physical_device_provoking_vertex_properties_EXT = 1000254002,
    surface_full_screen_exclusive_info_EXT = 1000255000,
    surface_capabilities_full_screen_exclusive_EXT = 1000255002,
    surface_full_screen_exclusive_win32_info_EXT = 1000255001,
    headless_surface_create_info_EXT = 1000256000,
    physical_device_line_rasterization_features_EXT = 1000259000,
    pipeline_rasterization_line_state_create_info_EXT = 1000259001,
    physical_device_line_rasterization_properties_EXT = 1000259002,
    physical_device_shader_atomic_float_features_EXT = 1000260000,
    physical_device_index_type_uint8_features_EXT = 1000265000,
    physical_device_extended_dynamic_state_features_EXT = 1000267000,
    physical_device_pipeline_executable_properties_features_KHR = 1000269000,
    pipeline_info_KHR = 1000269001,
    pipeline_executable_properties_KHR = 1000269002,
    pipeline_executable_info_KHR = 1000269003,
    pipeline_executable_statistic_KHR = 1000269004,
    pipeline_executable_internal_representation_KHR = 1000269005,
    physical_device_shader_demote_to_helper_invocation_features_EXT = 1000276000,
    physical_device_device_generated_commands_properties_NV = 1000277000,
    graphics_shader_group_create_info_NV = 1000277001,
    graphics_pipeline_shader_groups_create_info_NV = 1000277002,
    indirect_commands_layout_token_NV = 1000277003,
    indirect_commands_layout_create_info_NV = 1000277004,
    generated_commands_info_NV = 1000277005,
    generated_commands_memory_requirements_info_NV = 1000277006,
    physical_device_device_generated_commands_features_NV = 1000277007,
    physical_device_inherited_viewport_scissor_features_NV = 1000278000,
    command_buffer_inheritance_viewport_scissor_info_NV = 1000278001,
    physical_device_texel_buffer_alignment_features_EXT = 1000281000,
    physical_device_texel_buffer_alignment_properties_EXT = 1000281001,
    command_buffer_inheritance_render_pass_transform_info_QCOM = 1000282000,
    render_pass_transform_begin_info_QCOM = 1000282001,
    physical_device_device_memory_report_features_EXT = 1000284000,
    device_memory_report_create_info_EXT = 1000284001,
    device_memory_report_callback_data_EXT = 1000284002,
    physical_device_robustness2_features_EXT = 1000286000,
    physical_device_robustness2_properties_EXT = 1000286001,
    sampler_custom_border_color_create_info_EXT = 1000287000,
    physical_device_custom_border_color_properties_EXT = 1000287001,
    physical_device_custom_border_color_features_EXT = 1000287002,
    pipeline_library_create_info_KHR = 1000290000,
    physical_device_private_data_features_EXT = 1000295000,
    device_private_data_create_info_EXT = 1000295001,
    private_data_slot_create_info_EXT = 1000295002,
    physical_device_pipeline_creation_cache_control_features_EXT = 1000297000,
    physical_device_diagnostics_config_features_NV = 1000300000,
    device_diagnostics_config_create_info_NV = 1000300001,
    memory_barrier2_KHR = 1000314000,
    buffer_memory_barrier2_KHR = 1000314001,
    image_memory_barrier2_KHR = 1000314002,
    dependency_info_KHR = 1000314003,
    submit_info2_KHR = 1000314004,
    semaphore_submit_info_KHR = 1000314005,
    command_buffer_submit_info_KHR = 1000314006,
    physical_device_synchronization2_features_KHR = 1000314007,
    queue_family_checkpoint_properties2_NV = 1000314008,
    checkpoint_data2_NV = 1000314009,
    physical_device_zero_initialize_workgroup_memory_features_KHR = 1000325000,
    physical_device_fragment_shading_rate_enums_properties_NV = 1000326000,
    physical_device_fragment_shading_rate_enums_features_NV = 1000326001,
    pipeline_fragment_shading_rate_enum_state_create_info_NV = 1000326002,
    physical_device_ycbcr_2plane_444_formats_features_EXT = 1000330000,
    physical_device_fragment_density_map2_features_EXT = 1000332000,
    physical_device_fragment_density_map2_properties_EXT = 1000332001,
    copy_command_transform_info_QCOM = 1000333000,
    physical_device_image_robustness_features_EXT = 1000335000,
    physical_device_workgroup_memory_explicit_layout_features_KHR = 1000336000,
    copy_buffer_info2_KHR = 1000337000,
    copy_image_info2_KHR = 1000337001,
    copy_buffer_to_image_info2_KHR = 1000337002,
    copy_image_to_buffer_info2_KHR = 1000337003,
    blit_image_info2_KHR = 1000337004,
    resolve_image_info2_KHR = 1000337005,
    buffer_copy2_KHR = 1000337006,
    image_copy2_KHR = 1000337007,
    image_blit2_KHR = 1000337008,
    buffer_image_copy2_KHR = 1000337009,
    image_resolve2_KHR = 1000337010,
    physical_device_x4444_formats_features_EXT = 1000340000,
    directfb_surface_create_info_EXT = 1000346000,
    physical_device_mutable_descriptor_type_features_VALVE = 1000351000,
    mutable_descriptor_type_create_info_VALVE = 1000351002,
    physical_device_vertex_input_dynamic_state_features_EXT = 1000352000,
    vertex_input_binding_description2_EXT = 1000352001,
    vertex_input_attribute_description2_EXT = 1000352002,
    import_memory_zircon_handle_info_FUCHSIA = 1000364000,
    memory_zircon_handle_properties_FUCHSIA = 1000364001,
    memory_get_zircon_handle_info_FUCHSIA = 1000364002,
    import_semaphore_zircon_handle_info_FUCHSIA = 1000365000,
    semaphore_get_zircon_handle_info_FUCHSIA = 1000365001,
    physical_device_extended_dynamic_state2_features_EXT = 1000377000,
    screen_surface_create_info_QNX = 1000378000,
    physical_device_color_write_enable_features_EXT = 1000381000,
    pipeline_color_write_create_info_EXT = 1000381001
  };
  enum class object_type : u32 {
    unknown = 0,
    instance = 1,
    physical_device = 2,
    device = 3,
    queue = 4,
    semaphore = 5,
    command_buffer = 6,
    fence = 7,
    device_memory = 8,
    buffer = 9,
    image = 10,
    event = 11,
    query_pool = 12,
    buffer_view = 13,
    image_view = 14,
    shader_module = 15,
    pipeline_cache = 16,
    pipeline_layout = 17,
    render_pass = 18,
    pipeline = 19,
    descriptor_set_layout = 20,
    sampler = 21,
    descriptor_pool = 22,
    descriptor_set = 23,
    framebuffer = 24,
    command_pool = 25,
    sampler_ycbcr_conversion = 1000156000,
    descriptor_update_template = 1000085000,
    surface_KHR = 1000000000,
    swapchain_KHR = 1000001000,
    display_KHR = 1000002000,
    display_mode_KHR = 1000002001,
    debug_report_callback_EXT = 1000011000,
    video_session_KHR = 1000023000,
    video_session_parameters_KHR = 1000023001,
    debug_utils_messenger_EXT = 1000128000,
    acceleration_structure_KHR = 1000150000,
    validation_cache_EXT = 1000160000,
    acceleration_structure_NV = 1000165000,
    performance_configuration_INTEL = 1000210000,
    deferred_operation_KHR = 1000268000,
    indirect_commands_layout_NV = 1000277000,
    private_data_slot_EXT = 1000295000
  };


  enum class format {
    undefined = 0,
    r4g4_unorm_pack8 = 1,
    r4g4b4a4_unorm_pack16 = 2,
    b4g4r4a4_unorm_pack16 = 3,
    r5g6b5_unorm_pack16 = 4,
    b5g6r5_unorm_pack16 = 5,
    r5g5b5a1_unorm_pack16 = 6,
    b5g5r5a1_unorm_pack16 = 7,
    a1r5g5b5_unorm_pack16 = 8,
    r8_unorm = 9,
    r8_norm = 10,
    r8_uscaled = 11,
    r8_scaled = 12,
    r8_uint = 13,
    r8_int = 14,
    r8_srgb = 15,
    r8g8_unorm = 16,
    r8g8_norm = 17,
    r8g8_uscaled = 18,
    r8g8_scaled = 19,
    r8g8_uint = 20,
    r8g8_int = 21,
    r8g8_srgb = 22,
    r8g8b8_unorm = 23,
    r8g8b8_norm = 24,
    r8g8b8_uscaled = 25,
    r8g8b8_scaled = 26,
    r8g8b8_uint = 27,
    r8g8b8_int = 28,
    r8g8b8_srgb = 29,
    b8g8r8_unorm = 30,
    b8g8r8_norm = 31,
    b8g8r8_uscaled = 32,
    b8g8r8_scaled = 33,
    b8g8r8_uint = 34,
    b8g8r8_int = 35,
    b8g8r8_srgb = 36,
    r8g8b8a8_unorm = 37,
    r8g8b8a8_norm = 38,
    r8g8b8a8_uscaled = 39,
    r8g8b8a8_scaled = 40,
    r8g8b8a8_uint = 41,
    r8g8b8a8_int = 42,
    r8g8b8a8_srgb = 43,
    b8g8r8a8_unorm = 44,
    b8g8r8a8_norm = 45,
    b8g8r8a8_uscaled = 46,
    b8g8r8a8_scaled = 47,
    b8g8r8a8_uint = 48,
    b8g8r8a8_int = 49,
    b8g8r8a8_srgb = 50,
    a8b8g8r8_unorm_pack32 = 51,
    a8b8g8r8_norm_pack32 = 52,
    a8b8g8r8_uscaled_pack32 = 53,
    a8b8g8r8_scaled_pack32 = 54,
    a8b8g8r8_uint_pack32 = 55,
    a8b8g8r8_int_pack32 = 56,
    a8b8g8r8_srgb_pack32 = 57,
    a2r10g10b10_unorm_pack32 = 58,
    a2r10g10b10_norm_pack32 = 59,
    a2r10g10b10_uscaled_pack32 = 60,
    a2r10g10b10_scaled_pack32 = 61,
    a2r10g10b10_uint_pack32 = 62,
    a2r10g10b10_int_pack32 = 63,
    a2b10g10r10_unorm_pack32 = 64,
    a2b10g10r10_norm_pack32 = 65,
    a2b10g10r10_uscaled_pack32 = 66,
    a2b10g10r10_scaled_pack32 = 67,
    a2b10g10r10_uint_pack32 = 68,
    a2b10g10r10_int_pack32 = 69,
    r16_unorm = 70,
    r16_norm = 71,
    r16_uscaled = 72,
    r16_scaled = 73,
    r16_uint = 74,
    r16_int = 75,
    r16_float = 76,
    r16g16_unorm = 77,
    r16g16_norm = 78,
    r16g16_uscaled = 79,
    r16g16_scaled = 80,
    r16g16_uint = 81,
    r16g16_int = 82,
    r16g16_float = 83,
    r16g16b16_unorm = 84,
    r16g16b16_norm = 85,
    r16g16b16_uscaled = 86,
    r16g16b16_scaled = 87,
    r16g16b16_uint = 88,
    r16g16b16_int = 89,
    r16g16b16_float = 90,
    r16g16b16a16_unorm = 91,
    r16g16b16a16_norm = 92,
    r16g16b16a16_uscaled = 93,
    r16g16b16a16_scaled = 94,
    r16g16b16a16_uint = 95,
    r16g16b16a16_int = 96,
    r16g16b16a16_float = 97,
    r32_uint = 98,
    r32_int = 99,
    r32_float = 100,
    r32g32_uint = 101,
    r32g32_int = 102,
    r32g32_float = 103,
    r32g32b32_uint = 104,
    r32g32b32_int = 105,
    r32g32b32_float = 106,
    r32g32b32a32_uint = 107,
    r32g32b32a32_int = 108,
    r32g32b32a32_float = 109,
    r64_uint = 110,
    r64_int = 111,
    r64_float = 112,
    r64g64_uint = 113,
    r64g64_int = 114,
    r64g64_float = 115,
    r64g64b64_uint = 116,
    r64g64b64_int = 117,
    r64g64b64_float = 118,
    r64g64b64a64_uint = 119,
    r64g64b64a64_int = 120,
    r64g64b64a64_float = 121,
    b10g11r11_ufloat_pack32 = 122,
    e5b9g9r9_ufloat_pack32 = 123,
    d16_unorm = 124,
    x8d24_unorm_pack32 = 125,
    d32_float = 126,
    s8_uint = 127,
    d16_unorm_s8_uint = 128,
    d24_unorm_s8_uint = 129,
    d32_float_s8_uint = 130,
    bc1_rgb_unorm_block = 131,
    bc1_rgb_srgb_block = 132,
    bc1_rgba_unorm_block = 133,
    bc1_rgba_srgb_block = 134,
    bc2_unorm_block = 135,
    bc2_srgb_block = 136,
    bc3_unorm_block = 137,
    bc3_srgb_block = 138,
    bc4_unorm_block = 139,
    bc4_norm_block = 140,
    bc5_unorm_block = 141,
    bc5_norm_block = 142,
    bc6h_ufloat_block = 143,
    bc6h_float_block = 144,
    bc7_unorm_block = 145,
    bc7_srgb_block = 146,
    etc2_r8g8b8_unorm_block = 147,
    etc2_r8g8b8_srgb_block = 148,
    etc2_r8g8b8a1_unorm_block = 149,
    etc2_r8g8b8a1_srgb_block = 150,
    etc2_r8g8b8a8_unorm_block = 151,
    etc2_r8g8b8a8_srgb_block = 152,
    eac_r11_unorm_block = 153,
    eac_r11_norm_block = 154,
    eac_r11g11_unorm_block = 155,
    eac_r11g11_norm_block = 156,
    astc_4x4_unorm_block = 157,
    astc_4x4_srgb_block = 158,
    astc_5x4_unorm_block = 159,
    astc_5x4_srgb_block = 160,
    astc_5x5_unorm_block = 161,
    astc_5x5_srgb_block = 162,
    astc_6x5_unorm_block = 163,
    astc_6x5_srgb_block = 164,
    astc_6x6_unorm_block = 165,
    astc_6x6_srgb_block = 166,
    astc_8x5_unorm_block = 167,
    astc_8x5_srgb_block = 168,
    astc_8x6_unorm_block = 169,
    astc_8x6_srgb_block = 170,
    astc_8x8_unorm_block = 171,
    astc_8x8_srgb_block = 172,
    astc_10x5_unorm_block = 173,
    astc_10x5_srgb_block = 174,
    astc_10x6_unorm_block = 175,
    astc_10x6_srgb_block = 176,
    astc_10x8_unorm_block = 177,
    astc_10x8_srgb_block = 178,
    astc_10x10_unorm_block = 179,
    astc_10x10_srgb_block = 180,
    astc_12x10_unorm_block = 181,
    astc_12x10_srgb_block = 182,
    astc_12x12_unorm_block = 183,
    astc_12x12_srgb_block = 184,
    // Provided by VK_VERSION_1_1
    g8b8g8r8_422_unorm = 1000156000,
    b8g8r8g8_422_unorm = 1000156001,
    g8_b8_r8_3plane_420_unorm = 1000156002,
    g8_b8r8_2plane_420_unorm = 1000156003,
    g8_b8_r8_3plane_422_unorm = 1000156004,
    g8_b8r8_2plane_422_unorm = 1000156005,
    g8_b8_r8_3plane_444_unorm = 1000156006,
    r10x6_unorm_pack16 = 1000156007,
    r10x6g10x6_unorm_2pack16 = 1000156008,
    r10x6g10x6b10x6a10x6_unorm_4pack16 = 1000156009,
    g10x6b10x6g10x6r10x6_422_unorm_4pack16 = 1000156010,
    b10x6g10x6r10x6g10x6_422_unorm_4pack16 = 1000156011,
    g10x6_b10x6_r10x6_3plane_420_unorm_3pack16 = 1000156012,
    g10x6_b10x6r10x6_2plane_420_unorm_3pack16 = 1000156013,
    g10x6_b10x6_r10x6_3plane_422_unorm_3pack16 = 1000156014,
    g10x6_b10x6r10x6_2plane_422_unorm_3pack16 = 1000156015,
    g10x6_b10x6_r10x6_3plane_444_unorm_3pack16 = 1000156016,
    r12x4_unorm_pack16 = 1000156017,
    r12x4g12x4_unorm_2pack16 = 1000156018,
    r12x4g12x4b12x4a12x4_unorm_4pack16 = 1000156019,
    g12x4b12x4g12x4r12x4_422_unorm_4pack16 = 1000156020,
    b12x4g12x4r12x4g12x4_422_unorm_4pack16 = 1000156021,
    g12x4_b12x4_r12x4_3plane_420_unorm_3pack16 = 1000156022,
    g12x4_b12x4r12x4_2plane_420_unorm_3pack16 = 1000156023,
    g12x4_b12x4_r12x4_3plane_422_unorm_3pack16 = 1000156024,
    g12x4_b12x4r12x4_2plane_422_unorm_3pack16 = 1000156025,
    g12x4_b12x4_r12x4_3plane_444_unorm_3pack16 = 1000156026,
    g16b16g16r16_422_unorm = 1000156027,
    b16g16r16g16_422_unorm = 1000156028,
    g16_b16_r16_3plane_420_unorm = 1000156029,
    g16_b16r16_2plane_420_unorm = 1000156030,
    g16_b16_r16_3plane_422_unorm = 1000156031,
    g16_b16r16_2plane_422_unorm = 1000156032,
    g16_b16_r16_3plane_444_unorm = 1000156033,


    // Provided by VK_IMG_format_pvrtc
    pvrtc_12bpp_unorm_block_IMG = 1000054000,
    pvrtc_14bpp_unorm_block_IMG = 1000054001,
    pvrtc_22bpp_unorm_block_IMG = 1000054002,
    pvrtc_24bpp_unorm_block_IMG = 1000054003,
    pvrtc_12bpp_srgb_block_IMG = 1000054004,
    pvrtc_14bpp_srgb_block_IMG = 1000054005,
    pvrtc_22bpp_srgb_block_IMG = 1000054006,
    pvrtc_24bpp_srgb_block_IMG = 1000054007,

    // Provided by VK_EXT_texture_compression_astc_hdr
    astc_4x4_float_block_EXT   = 1000066000,
    astc_5x4_float_block_EXT   = 1000066001,
    astc_5x5_float_block_EXT   = 1000066002,
    astc_6x5_float_block_EXT   = 1000066003,
    astc_6x6_float_block_EXT   = 1000066004,
    astc_8x5_float_block_EXT   = 1000066005,
    astc_8x6_float_block_EXT   = 1000066006,
    astc_8x8_float_block_EXT   = 1000066007,
    astc_10x5_float_block_EXT  = 1000066008,
    astc_10x6_float_block_EXT  = 1000066009,
    astc_10x8_float_block_EXT  = 1000066010,
    astc_10x10_float_block_EXT = 1000066011,
    astc_12x10_float_block_EXT = 1000066012,
    astc_12x12_float_block_EXT = 1000066013,

    // Provided by VK_EXT_4444_formats
    a4r4g4b4_unorm_pack16_EXT = 1000340000,
    a4b4g4r4_unorm_pack16_EXT = 1000340001,
  };
  enum class vendor_id : u32{
    viv = 0x10001,
    vsi = 0x10002,
    kazan = 0x10003,
    codeplay = 0x10004,
    mesa = 0x10005,
    pocl = 0x10006,
  };
  enum class pipeline_cache_header_version : u32{
    one = 1
  };
  enum class system_allocation_scope : u32{
    command  = 0,
    object   = 1,
    cache    = 2,
    device   = 3,
    instance = 4,
  };
  enum class internal_allocation_type : u32{
    executable = 0
  };
  enum class image_tiling : u32{
    optimal = 0,
    linear = 1,
    drm_format_modifier_EXT = 1000158000
  };
  enum class image_type : u32{
    d1,
    d2,
    d3
  };
  enum class physical_device_type : u32 {
    other = 0,
    integrated_gpu = 1,
    discrete_gpu = 2,
    virtual_gpu = 3,
    cpu = 4
  };
  enum class query_type : u32 {
    occlusion = 0,
    pipeline_statistics = 1,
    timestamp = 2,
    result_status_only_KHR = 1000023000,
    video_encode_bitstream_buffer_range_KHR = 1000299000,
    transform_feedback_stream_EXT = 1000028004,
    performance_query_KHR = 1000116000,
    acceleration_structure_compacted_size_KHR = 1000150000,
    acceleration_structure_serialization_size_KHR = 1000150001,
    acceleration_structure_compacted_size_NV = 1000165000,
    performance_query_INTEL = 1000210000
  };
  enum class image_view_type : u32 {
    d1,
    d2,
    d3,
    cube,
    d1_array,
    d2_array,
    cube_array
  };
  enum class blend_factor : u32 {
    zero,
    one,
    src_color,
    one_minus_src_color,
    dst_color,
    src_alpha,
    one_minus_src_alpha,
    dst_alpha,
    one_minus_dst_alpha,
    constant_color,
    one_minus_constant_color,
    constant_alpha,
    one_minus_constant_alpha,
    src_alpha_saturate,
    src1_color,
    one_minus_src1_color,
    src1_alpha,
    one_minus_sr1c_alpha
  };
  enum class blend_op : u32 {
    add = 0,
    subtract = 1,
    reverse_subtract = 2,
    min = 3,
    max = 4,

    // Provided by VK_EXT_blend_operation_advanced

    zero = 1000148000,
    src = 1000148001,
    dst = 1000148002,
    src_over = 1000148003,
    dst_over = 1000148004,
    src_in = 1000148005,
    dst_in = 1000148006,
    src_out = 1000148007,
    dst_out = 1000148008,
    src_a_top = 1000148009,
    dst_a_top = 1000148010,
    Xor = 1000148011,
    multiply = 1000148012,
    screen = 1000148013,
    overlay = 1000148014,
    darken = 1000148015,
    lighten = 1000148016,
    color_dodge = 1000148017,
    color_burn = 1000148018,
    hardlight = 1000148019,
    softlight = 1000148020,
    difference = 1000148021,
    exclusion = 1000148022,
    invert = 1000148023,
    invert_RGB = 1000148024,
    linear_dodge = 1000148025,
    linear_burn = 1000148026,
    vivid_light = 1000148027,
    linear_light = 1000148028,
    pin_light = 1000148029,
    hard_mix = 1000148030,
    hsl_hue = 1000148031,
    hsl_saturation = 1000148032,
    hsl_color = 1000148033,
    hsl_luminosity = 1000148034,
    plus = 1000148035,
    plus_clamped = 1000148036,
    plus_clamped_alpha = 1000148037,
    plus_darker = 1000148038,
    minus = 1000148039,
    minus_clamped = 1000148040,
    contrast = 1000148041,
    invert_ovg = 1000148042,
    red = 1000148043,
    green = 1000148044,
    blue = 1000148045,
  };
  enum class compare_op : u32 {
    never,
    less,
    equal,
    less_or_equal,
    greater,
    not_equal,
    greater_or_equal,
    always
  };
  enum class dynamic_state : u32 {
    viewport = 0,
    scissor = 1,
    line_width = 2,
    depth_bias = 3,
    blend_constants = 4,
    depth_bounds = 5,
    stencil_compare_mask = 6,
    stencil_write_mask = 7,
    stencil_reference = 8,

    // Provided by VK_NV_clip_space_w_scaling
    viewport_with_scaling_NV = 1000087000,

    // Provided by VK_EXT_discard_rectangles
    discard_rectangle = 1000099000,

    // Provided by VK_EXT_sample_locations
    sample_locations_EXT = 1000143000,

    // Provided by VK_KHR_ray_tracing
    ray_tracing_pipeline_stack_size_KHR = 1000347000,

    // Provided by VK_NV_shading_rate_image
    viewport_shading_rate_palette_NV = 1000164004,
    viewport_coarse_sample_order_NV = 1000164006,

    // Provided by VK_NV_scissor_exclusive
    exclusive_scissor_NV = 1000205001,

    // Provided by VK_KHR_fragment_shading_rate
    fragment_shading_rate_KHR = 1000226000,

    // Provided by VK_EXT_line_rasterization
    line_stipple_EXT = 1000259000,

    // Provided by VK_EXT_extended_dynamic_state
    cull_mode_EXT = 1000267000,
    front_face_EXT = 1000267001,
    primitive_topology_EXT = 1000267002,
    viewport_with_count_EXT = 1000267003,
    scissor_with_count_EXT = 1000267004,
    vertex_input_binding_stride_EXT = 1000267005,
    depth_test_enable_EXT = 1000267006,
    depth_write_enable_EXT = 1000267007,
    depth_compare_op_EXT = 1000267008,
    depth_bounds_test_enable_EXT = 1000267009,
    stencil_test_enable_EXT = 1000267010,
    stencil_op_EXT = 1000267011,

    // provided by VK_EXT_extended_dynamic_state2

    vertex_input_EXT = 1000352000,
    patch_control_points_EXT = 1000377000,
    rasterizer_discard_enable_EXT = 1000377001,
    depth_bias_enable_EXT = 1000377002,
    logic_op_EXT = 1000377003,
    primitive_restart_enable_EXT = 1000377004,

    // provided by VK_EXT_color_write

    color_write_enable_EXT = 1000381000
  };
  enum class front_face : u32 {
    counter_clockwise,
    clockwise
  };
  enum class vertex_input_rate : u32 {
    vertex = 0,
    instance = 1
  };
  enum class primitive_topology : u32 {
    point_list,
    line_list,
    line_strip,
    triangle_list,
    triangle_strip,
    triangle_fan,
    line_list_with_adjacency,
    line_strip_with_adjacency,
    triangle_list_with_adjacency,
    triangle_strip_with_adjacency,
    patch_list
  };
  enum class polygon_mode : u32 {
    fill,
    line,
    point,
    fill_rectangle_NV = 1000153000
  };
  enum class stencil_op : u32 {
    keep,
    zero,
    replace,
    increment_and_clamp,
    decrement_and_clamp,
    invert,
    increment_and_wrap,
    decrement_and_wrap
  };
  enum class logic_op : u32 {
    clear,
    and_op,
    and_reverse,
    copy,
    and_inverted,
    noop,
    xor_op,
    or_op,
    not_op,
    equivalent,
    invert,
    or_reverse,
    copy_inverted,
    or_inverted,
    not_and,
    set
  };
  /*enum class blend_overlap : u32 {
    uncorrelated,
    disjoint,
    conjoint
  };*/
  enum class border_color : u32 {
    float_transparent_black = 0,
    int_transparent_black = 1,
    float_opaque_black = 2,
    int_opaque_black = 3,
    float_opaque_white = 4,
    int_opaque_white = 5,
    float_custom_EXT = 1000287003,
    int_custom_EXT = 1000287004,
  };
  enum class filter : u32 {
    nearest = 0,
    linear = 1,
    cubic_IMG = 1000015000
  };
  enum class sampler_address_mode : u32 {
    repeat = 0,
    mirrored_repeat = 1,
    clamp_to_edge = 2,
    clamp_to_border = 3,
    mirror_clamp_to_edge = 4
  };
  enum class sampler_mipmap_mode : u32 {
    nearest = 0,
    linear  = 1
  };
  enum class descriptor_type : u32 {
    sampler = 0,
    combined_image_sampler = 1,
    sampled_image = 2,
    storage_image = 3,
    uniform_texel_buffer = 4,
    storage_texel_buffer = 5,
    uniform_buffer = 6,
    storage_buffer = 7,
    uniform_buffer_dynamic = 8,
    storage_buffer_dynamic = 9,
    input_attachment = 10,

    // Provided by VK_EXT_inline_uniform_block
    inline_uniform_block_EXT = 1000138000,
    // Provided by VK_KHR_ray_tracing
    acceleration_structure_KHR = 1000150000,
    // Provided by VK_NV_ray_tracing
    acceleration_structure_NV = 1000165000,
    // Provided by the Valve extension lmao
    mutable_VALVE = 1000351000
  };
  enum class attachment_load_op : u32 {
    load,
    clear,
    dont_care
  };
  enum class attachment_store_op : u32 {
    store,
    dont_care,
    none_QCOM = 1000301000
  };
  enum class pipeline_bind_point : u32 {
    graphics = 0,
    compute = 1,
    ray_tracing_KHR = 1000165000
  };
  enum class command_buffer_level : u32 {
    primary = 0,
    secondary = 1
  };
  enum class index_type : u32 {
    uint16 = 0,
    uint32 = 1,
    none_KHR = 1000165000,
    uint8_EXT = 1000265000
  };
  enum class subpass_contents : u32 {
    inlined = 0,
    secondary_command_buffers = 1
  };
  enum class sharing_mode {
    exclusive,
    concurrent
  };

  enum class component_swizzle : u32 {
    identity = 0,
    zero = 1,
    one = 2,
    r = 3,
    g = 4,
    b = 5,
    a = 6,
  };





  // vulkan 1.1

  enum class point_clipping_behaviour : u32{
    all_clip_planes = 0,
    user_clip_planes_only = 1
  };
  enum class tessellation_domain_origin : u32 {
    upper_left = 0,
    lower_left = 1
  };
  enum class sampler_ycbcr_model_conversion : u32 {
    rgb_identity = 0,
    ycbcr_identity = 1,
    ycbcr_709 = 2,
    ycbcr_601 = 3,
    ycbcr_2020 = 4
  };
  enum class sampler_ycbcr_range : u32 {
    itu_full = 0,
    itu_narrow = 1
  };
  enum class chroma_location : u32 {
    cosited_even = 0,
    midpoint     = 1
  };
  enum class descriptor_update_template_type : u32 {
    descriptor_set       = 0,
    push_descriptors_KHR = 1
  };



  // vulkan 1.2

  enum class driver_id : u32 {
    amd_proprietary = 1,
    amd_open_source = 2,
    mesa_radv = 3,
    nvidia_proprietary = 4,
    intel_proprietary_windows = 5,
    intel_open_source_mesa = 6,
    imagination_proprietary = 7,
    qualcomm_proprietary = 8,
    arm_proprietary = 9,
    google_swiftshader = 10,
    ggp_proprietary = 11,
    broadcom_proprietary = 12,
    mesa_llvmpipe = 13,
    moltenvk = 14,
    coreavi_proprietary = 15
  };
  enum class shader_float_controls_independence : u32 {
    only_32_bit = 0,
    all = 1,
    none = 2
  };
  enum class sampler_reduction_mode : u32 {
    weighted_average = 0,
    min = 1,
    max = 2
  };
  enum class semaphore_type : u32 {
    binary   = 0,
    timeline = 1
  };




  // VK_KHR_surface

  enum class present_mode_KHR : u32 {
    immediate = 0,
    mailbox = 1,
    fifo = 2,
    fifo_relaxed = 3,
    shared_demand_refresh = 1000111000,
    shared_continuous_refresh = 1000111001
  };
  enum class colorspace_KHR : u32 {
    srgb_nonlinear = 0,
    display_p3_nonlinear = 1000104001,
    extended_srgb_linear = 1000104002,
    display_p3_linear = 1000104003,
    dcip3_nonlinear = 1000104004,
    bt709_linear = 1000104005,
    bt709_nonlinear = 1000104006,
    bt2020_linear = 1000104007,
    hdr10_st2084 = 1000104008,
    dolby_vision = 1000104009,
    hdr10_hlg = 1000104010,
    adobe_rgb_linear = 1000104011,
    adobe_rgb_nonlinear = 1000104012,
    pass_through = 1000104013,
    extended_srgb_nonlinear = 1000104014,
    display_native_AMD = 1000213000
  };



  // VK_KHR_performance_counter

  enum class performance_counter_unit_KHR : u32 {
    generic = 0,
    percentage = 1,
    nanoseconds = 2,
    bytes = 3,
    bytes_per_second = 4,
    kelvin = 5,
    watts = 6,
    volts = 7,
    amps = 8,
    hertz = 9,
    cycles = 10
  };
  enum class performance_counter_scope_KHR : u32 {
    command_buffer = 0,
    render_pass = 1,
    command = 2
  };
  enum class performance_counter_storage_KHR : u32 {
    int32 = 0,
    int64 = 1,
    uint32 = 2,
    uint64 = 3,
    float32 = 4,
    float64 = 5
  };


  // VK_KHR_fragment_shading_rate

  enum class fragment_shading_rate_combiner_op_KHR : u32 {
    keep = 0,
    replace = 1,
    min = 2,
    max = 3,
    mul = 4,
  };


  // VK_KHR_pipeline_executable_properties

  enum class pipeline_executable_statistic_format_KHR : u32 {
    bool32 = 0,
    int64 = 1,
    uint64 = 2,
    float64 = 3
  };


  // VK_AMD_rasterization_order

  enum class rasterization_order_AMD : u32 {
    strict = 0,
    relaxed = 1
  };


  // VK_EXT_validation_flags

  enum class validation_check_EXT : u32 {
    all = 0,
    shaders = 1
  };


  // VK_EXT_display_control

  enum class display_power_state : u32 {
    off = 0,
    suspend = 1,
    on = 2
  };
  enum class device_event_type : u32 {
    display_hotplug = 0
  };
  enum class display_event_type_EXT : u32 {
    first_pixel_out = 0
  };


  // VK_NV_viewport_swizzle

  enum class viewport_component_swizzle_NV : u32 {
    positive_x = 0,
    negative_x = 1,
    positive_y = 2,
    negative_y = 3,
    positive_z = 4,
    negative_z = 5,
    positive_w = 6,
    negative_w = 7
  };


  // VK_EXT_discard_rectangles

  enum class discard_rectangle_mode_EXT : u32 {
    inclusive = 0,
    exclusive = 1
  };


  // VK_EXT_conservative_rasterization

  enum class conservative_rasterization_mode_EXT : u32 {
    disabled = 0,
    overestimate = 1,
    underestimate = 2
  };


  // VK_EXT_blend_operation_advanced

  enum class blend_overlap_EXT : u32 {
    uncorrelated = 0,
    disjoint = 1,
    conjoint = 2
  };


  // VK_NV_framebuffer_mixed_samples

  enum class coverage_modulation_mode_NV : u32 {
    none = 0,
    rgb = 1,
    alpha = 2,
    rgba = 3
  };


  // VK_EXT_validation_cache

  enum class validation_cache_header_version_EXT : u32 {
    one = 1
  };


  // VK_NV_shading_rate_image

  enum class shading_rate_palette_entry_NV : u32 {
    no_invocations = 0,
    x16_per_pixel = 1,
    x8_per_pixel = 2,
    x4_per_pixel = 3,
    x2_per_pixel = 4,
    once_per_pixel = 5,
    once_per_2x1_pixels = 6,
    once_per_1x2_pixels = 7,
    once_per_2x2_pixels = 8,
    once_per_4x2_pixels = 9,
    once_per_2x4_pixels = 10,
    once_per_4x4_pixels = 11
  };
  enum class coarse_sample_order_type_NV : u32 {
    custom      = 1,
    pixel_major  = 2,
    sample_major = 3
  };


  // VK_KHR_ray_tracing_pipeline

  enum class ray_tracing_shader_group_type_KHR : u32 {
    general = 0,
    triangles_hit_group = 1,
    procedural_hit_group = 2
  };
  enum class shader_group_shader_KHR : u32 {
    general = 0,
    closest_hit = 1,
    any_hit = 2,
    intersection = 3
  };


  // VK_KHR_acceleration_structure

  enum class acceleration_structure_build_type_KHR : u32 {
    host = 0,
    device = 1,
    host_or_device = 2
  };
  enum class acceleration_structure_compatibility_KHR : u32 {
    compatible = 0,
    incompatible = 1
  };
  enum class build_acceleration_structure_mode_KHR : u32 {
    build = 0,
    update = 1
  };
  enum class copy_acceleration_structure_mode_KHR : u32 {
    clone = 0,
    compact = 1,
    serialize = 2,
    deserialize = 3
  };
  enum class acceleration_structure_type_KHR : u32 {
    top_level = 0,
    bottom_level = 1,
    generic = 2
  };
  enum class geometry_type_KHR : u32 {
    triangles = 0,
    aabbs = 1,
    instances = 2
  };


  // VK_EXT_global_priority

  enum class queue_global_priority_EXT : u32 {
    low = 128,
    medium = 256,
    high = 512,
    realtime = 1024
  };


  // VK_EXT_calibrated_timestamps

  enum class time_domain_EXT : u32 {
    device = 0,
    clock_monotonic = 1,
    clock_monotonic_raw = 2,
    query_performance_counter = 3
  };


  // VK_AMD_memory_overallocation_behavior

  enum class memory_overallocation_behavior_AMD : u32 {
    allowed = 1,
    disallowed = 2
  };


  // VK_INTEL_performance_query

  enum class performance_configuration_type_INTEL : u32 {
    command_queue_metrics_discovery_activated = 0
  };
  enum class query_pool_sampling_mode_INTEL : u32 {
    manual = 0
  };
  enum class performance_override_type_INTEL : u32 {
    null_hardware = 0,
    flush_gpu_caches = 1
  };
  enum class performance_parameter_type_INTEL : u32 {
    hw_counters_supported = 0,
    stream_marker_valid_bits = 1
  };
  enum class performance_value_type_INTEL : u32 {
    uint32 = 0,
    uint64 = 1,
    float32 = 2,
    boolean = 3,
    string = 4
  };


  // VK_EXT_validation_features

  enum class validation_feature_enable_EXT : u32 {
    gpu_assisted = 0,
    gpu_assisted_reserve_binding_slot = 1,
    best_practices = 2,
    debug_printf = 3,
    synchronization_validation = 4
  };
  enum class validation_feature_disable_EXT : u32 {
    all = 0,
    shaders = 1,
    thread_safety = 2,
    api_parameters = 3,
    object_lifetimes = 4,
    core_checks = 5,
    unique_handles = 6
  };


  // VK_NV_cooperative_matrix

  enum class matrix_component_type_NV : u32 {
    float16 = 0,
    float32 = 1,
    float64 = 2,
    int8 = 3,
    int16 = 4,
    int32 = 5,
    int64 = 6,
    uint8 = 7,
    uint16 = 8,
    uint32 = 9,
    uint64 = 10
  };
  enum class matrix_scope_NV : u32 {
    device = 1,
    workgroup = 2,
    subgroup = 3,
    queue_family = 5
  };


  // VK_NV_coverage_reduction_mode

  enum class coverage_reduction_mode_NV : u32 {
    merge = 0,
    truncate = 1
  };


  // VK_EXT_line_rasterization

  enum class line_rasterization_mode_EXT : u32 {
    rectangular = 1,
    bresenham = 2,
    rectangular_smooth = 3
  };


  // VK_NV_device_generated_commands

  enum class indirect_commands_token_type_NV : u32 {
    shader_group = 0,
    state_flags = 1,
    index_buffer = 2,
    vertex_buffer = 3,
    push_constant = 4,
    draw_indexed = 5,
    draw = 6,
    draw_tasks = 7
  };


  // VK_EXT_device_memory_report

  enum class device_memory_report_event_type_EXT : u32 {
    allocate = 0,
    free = 1,
    import = 2,
    unimport = 3,
    allocation_failed = 4
  };


  // VK_NV_fragment_shading_rate_enums

  enum class fragment_shading_rate_type_NV : u32 {
    fragment_size = 0,
    enums = 1
  };
  enum class fragment_shading_rate_NV : u32 {
    x1_invocation_per_pixel = 0,
    x1_invocation_per_1x2_pixels = 1,
    x1_invocation_per_2x1_pixels = 4,
    x1_invocation_per_2x2_pixels = 5,
    x1_invocation_per_2x4_pixels = 6,
    x1_invocation_per_4x2_pixels = 9,
    x1_invocation_per_4x4_pixels = 10,
    x2_invocations_per_pixel = 11,
    x4_invocations_per_pixel = 12,
    x8_invocations_per_pixel = 13,
    x16_invocations_per_pixel = 14,
    no_invocations = 15
  };

// #define VK_enable_vulkan_beta
#if defined(VK_enable_vulkan_beta)
  // VK_KHR_video_queue


  enum class query_result_status_KHR : i32 {
    error     = -1,
    not_ready = 0,
    complete  = 1
  };
  enum class video_codec_operation_KHR : u32 {
    invalid     = 0,
    encode_h264 = 0x00010000,
    decode_h264 = 0x00000001,
    decode_h265 = 0x00000002
  };
  enum class video_chroma_subsampling_KHR : u32 {
    invalid    = 0,
    monochrome = 0x00000001,
    x420       = 0x00000002,
    x422       = 0x00000004,
    x444       = 0x00000008
  };
  enum class video_component_bit_depth_KHR : u32 {
    invalid = 0,
    x8bit = 0x00000001,
    x10bit = 0x00000004,
    x12bit = 0x00000010
  };
  enum class video_capabilities_KHR : u32 {
    protected_content         = 0x00000001,
    separate_reference_images = 0x00000002
  };
  enum class video_session_create_KHR : u32 {
    protected_content = 0x00000001
  };
  enum class video_coding_control_KHR : u32 {
    reset_KHR = 0x00000001
  };
  enum class video_coding_quality_preset_KHR : u32 {
    normal  = 0x00000001,
    power   = 0x00000002,
    quality = 0x00000004
  };


  // VK_KHR_video_decode_queue

  enum class video_decode_KHR : u32 {
    VK_VIDEO_DECODE_RESERVED_0_BIT_KHR = 0x00000001
  };


  // VK_KHR_video_encode_queue

  enum class video_encode_KHR : u32 {
    VK_VIDEO_ENCODE_RESERVED_0_BIT_KHR = 0x00000001
  };
  enum class video_encode_rate_control_KHR : u32 {
    reset = 0x00000001
  };
  enum class video_encode_rate_control_mode_KHR : u32 {
    none = 0,
    cbr = 1,
    vbr = 2
  };


  // VK_EXT_video_encode_h264

  enum class video_encode_h264_capabilities_KHR : u32 {
    cabac = 0x00000001,
    cavlc = 0x00000002,
    weighted_bi_pred_implicit = 0x00000004,
    transform_8x8 = 0x00000008,
    chroma_qp_offset = 0x00000010,
    second_chroma_qp_offset = 0x00000020,
    deblocking_filter_disabled = 0x00000040,
    deblocking_filter_enabled = 0x00000080,
    deblocking_filter_partial = 0x00000100,
    multiple_slice_per_frame = 0x00000200,
    evenly_distributed_slice_size = 0x00000400
  };
  enum class video_encode_h264_input_mode_KHR : u32 {
    frame = 0x00000001,
    slice = 0x00000002,
    non_vcl = 0x00000004
  };
  enum class video_encode_h264_output_mode_KHR : u32 {
    frame = 0x00000001,
    slice = 0x00000002,
    non_vcl = 0x00000004
  };
  enum class video_encode_h264_create_KHR : u32 {
    VK_VIDEO_ENCODE_H264_CREATE_RESERVED_0_BIT_EXT = 0x00000001
  };


  // VK_EXT_video_decode_h264

  enum class video_decode_h264_field_layout_KHR : u32 {
    progressive_pictures_only = 0,
    line_interlaced_plane = 0x00000001,
    separate_interlaced_plane = 0x00000002
  };
#endif






  namespace flag_bits {

    enum class descriptor_set_layout {
      update_after_bind_pool = 0x2,
      push_descriptor = 0x1
    };
    enum class pipeline {
      disable_optimization = 0x1,
      allow_derivatives = 0x2,
      derivative = 0x4,
      view_index_from_device_index = 0x8,
      dispatch_base = 0x10,
      ray_tracing_no_null_any_hit_shaders = 0x4000,
      ray_tracing_no_null_closest_hit_shaders = 0x8000,
      ray_tracing_no_null_miss_shaders = 0x10000,
      ray_tracing_no_null_intersection_shaders = 0x20000,
      ray_tracing_skip_triangles = 0x1000,
      ray_tracing_skip_aabbs = 0x2000,
      defer_compile = 0x20,
      capture_statistics = 0x40,
      capture_internal_representations = 0x80,
      indirect_bindable = 0x40000,
      library = 0x800,
      fail_on_pipeline_compile_required = 0x100,
      early_return_on_failure = 0x200
    };
    enum class queue_create {
      graphics         = 0x1,
      compute          = 0x2,
      transfer         = 0x4,
      sparse_binding   = 0x8,
      protected_memory = 0x10,
      video_decode_KHR = 0x20,
      video_encode_KHR = 0x40
    };
    enum class command_pool {
      transient = 0x1,
      reset_command_buffer = 0x2,
      protected_memory = 0x4
    };
    enum class swapchain {
      split_instance_bind_regions = 0x1,
      protected_memory = 0x2,
      mutable_format = 0x4
    };
    enum class image {
      sparse_binding = 0x00000001,
      sparse_residency = 0x00000002,
      sparse_aliased = 0x00000004,
      mutable_format = 0x00000008,
      cube_compatible = 0x00000010,
      alias = 0x00000400,
      split_instance_bind_regions = 0x00000040,
      array_2d_compatible = 0x00000020,
      block_texel_view_compatible = 0x00000080,
      extended_usage = 0x00000100,
      protected_memory = 0x00000800,
      disjoint = 0x00000200,

      // Provided by VK_NV_corner_sampled_image
      corner_sampled_NV = 0x00002000,

      // Provided by VK_EXT_sample_locations
      sample_locations_compatible_depth_EXT = 0x00001000,

      // Provided by VK_EXT_fragment_density_map
      subsampled_EXT = 0x00004000
    };
    enum class image_view {

      // Provided by VK_EXT_fragment_density_map
      fragment_density_map_dynamic = 0x00000001,
      fragment_density_map_deferred = 0x00000002,
    };
    enum class buffer {
      sparse_binding = 0x00000001,
      sparse_residency = 0x00000002,
      sparse_aliased = 0x00000004,
      protected_memory = 0x00000008,
      device_address_capture_replay = 0x00000010,
    };
    enum class framebuffer {
      imageless = 0x1
    };
    enum class pipeline_cache {
      externally_synchronized = 0x1
    };
    enum class sampler {
      subsampled = 0x00000001,
      subsampled_coarse_reconstruction = 0x00000002
    };
    enum class descriptor_pool {
      free_descriptor_set = 0x00000001,
      update_after_bind = 0x00000002,
      host_only_VALVE   = 0x00000004
    };
    enum class render_pass {
      transform_QCOM = 0x2
    };
    enum class conditional_rendering {
      inverted = 0x1
    };
    enum class fence {
      signaled = 0x1
    };


    // Bitflags

    enum class cull_mode {
      none = 0x0,
      front = 0x1,
      back = 0x2,
      front_and_back = 0x3
    };
    enum class stencil_face{
      front = 0x1,
      back = 0x2,
      front_and_back = 0x3
    };
    enum class color_component {
      r = 0x1,
      g = 0x2,
      b = 0x4,
      a = 0x8
    };
    enum class sample_count {
      x1 = 0x1,
      x2 = 0x2,
      x4 = 0x4,
      x8 = 0x8,
      x16 = 0x10,
      x32 = 0x20,
      x64 = 0x40
    };
    enum class surface_transform {
      identity = 0x1,
      rotate90 = 0x2,
      rotate180 = 0x4,
      rotate270 = 0x8,
      horizontal_mirror = 0x10,
      horizontal_mirror_rotate90 = 0x20,
      horizontal_mirror_rotate180 = 0x40,
      horizontal_mirror_rotate270 = 0x80,
      inherit = 0x100
    };
    enum class composite_alpha {
      opaque = 0x1,
      pre_multiplied = 0x2,
      post_multiplied = 0x4,
      inherit = 0x8
    };
    enum class device_group_present_mode {
      local = 0x1,
      remote = 0x2,
      sum = 0x4,
      local_multi_device = 0x8
    };
    enum class pipeline_stage {
      top_of_pipe = 0x1,
      draw_indirect = 0x2,
      vertex_input = 0x4,
      vertex_shader = 0x8,
      tessellation_control_shader = 0x10,
      tessellation_evaluation_shader = 0x20,
      geometry_shader = 0x40,
      fragment_shader = 0x80,
      early_fragment_tests = 0x100,
      late_fragment_tests = 0x200,
      color_attachment_output = 0x400,
      compute_shader = 0x800,
      transfer = 0x1000,
      bottom_of_pipe = 0x2000,
      host = 0x4000,
      all_graphics = 0x8000,
      all_commands = 0x10000,
      transform_feedback = 0x0100'0000,
      conditional_rendering = 0x0004'0000,
      command_preprocess = 0x0002'0000,
      ray_tracing_shader = 0x0020'0000,
      acceleration_structure_build = 0x0200'0000,
      task_shader = 0x0008'0000,
      mesh_shader = 0x0010'0000,
      fragment_density_process = 0x0080'0000
    };
    enum class shader_stage {
      vertex = 0x1,
      tessellation_control = 0x2,
      tessellation_evaluation = 0x4,
      geometry = 0x8,
      fragment = 0x10,
      compute = 0x20,
      task = 0x40,
      mesh = 0x80,
      ray_gen = 0x100,
      any_hit = 0x200,
      closest_hit = 0x400,
      miss = 0x800,
      intersection = 0x1000,
      callable = 0x2000
    };

    enum class image_usage : u32{
      transfer_src = 0x1,
      transfer_dst = 0x2,
      sampled = 0x4,
      storage = 0x8,
      color_attachment = 0x10,
      depth_stencil_attachment = 0x20,
      transient_attachment = 0x40,
      input_attachment = 0x80,
      shading_rate_image = 0x100,
      fragment_density_map = 0x200,
      video_decode_dst_KHR = 0x400,
      video_decode_src_KHR = 0x800,
      video_decode_dpb_KHR = 0x1000,
      video_encode_dst_KHR = 0x2000,
      video_encode_src_KHR = 0x4000,
      video_encode_dpb_KHR = 0x8000,
    };
    enum class image_aspect : u32{
      color = 0x00000001,
      depth = 0x00000002,
      stencil = 0x00000004,
      metadata = 0x00000008,

      // Provided by VK_VERSION_1_1
      plane_0 = 0x00000010,
      plane_1 = 0x00000020,
      plane_2 = 0x00000040,

      // Provided by VK_EXT_image_drm_format_modifier
      memory_plane_0 = 0x00000080,
      memory_plane_1 = 0x00000100,
      memory_plane_2 = 0x00000200,
      memory_plane_3 = 0x00000400
    };
    enum class buffer_usage : u32 {
      transfer_bit = 0x1,
      transfer_dst = 0x2,
      uniform_texel_buffer = 0x4,
      storage_texel_buffer = 0x8,
      uniform_buffer = 0x10,
      storage_buffer = 0x20,
      index_buffer = 0x40,
      vertex_buffer = 0x80,
      indirect_buffer = 0x100,
      video_decode_src_KHR = 0x00002000,
      video_decode_dst_KHR = 0x00004000,
      video_encode_dst_KHR = 0x00008000,
      video_encode_src_KHR = 0x00010000,
      shader_device_address = 0x20000,
      transform_feedback_buffer_EXT = 0x800,
      transform_feedback_counter_buffer_EXT = 0x1000,
      conditional_rendering_EXT = 0x200,
      shader_binding_table_KHR = 0x400,
      acceleration_structure_build_input_read_only_KHR = 0x00080000,
      acceleration_structure_storage_KHR               = 0x00100000
    };

    enum class device_queue : u32 {
      protected_memory = 0x1
    };

    enum class performance_counter {
      performance_impacting = 0x00000001,
      concurrently_impacted = 0x00000002,
    };
    enum class format_feature {
      sampled_image = 0x00000001,
      storage_image = 0x00000002,
      storage_image_atomic = 0x00000004,
      uniform_texel_buffer = 0x00000008,
      storage_texel_buffer = 0x00000010,
      storage_texel_buffer_atomic = 0x00000020,
      vertex_buffer = 0x00000040,
      color_attachment = 0x00000080,
      color_attachment_blend = 0x00000100,
      depth_stencil_attachment = 0x00000200,
      blit_src = 0x00000400,
      blit_dst = 0x00000800,
      sampled_image_filter_linear = 0x00001000,
      transfer_src = 0x00004000,
      transfer_dst = 0x00008000,
      midpoint_chroma_samples = 0x00020000,
      sampled_image_ycbcr_conversion_linear_filter = 0x00040000,
      sampled_image_ycbcr_conversion_separate_reconstruction_filter = 0x00080000,
      sampled_image_ycbcr_conversion_chroma_reconstruction_explicit = 0x00100000,
      sampled_image_ycbcr_conversion_chroma_reconstruction_explicit_forceable = 0x00200000,
      disjoint = 0x00400000,
      cosited_chroma_samples = 0x00800000,
      sampled_image_filter_minmax = 0x00010000,

      // Provided by VK_IMG_filter_cubic
      sampled_image_filter_cubic_IMG = 0x00002000,

      // Provided by VK_KHR_acceleration_structure
      acceleration_structure_vertex_buffer_KHR = 0x20000000,

      // Provided by VK_EXT_fragment_density_map
      fragment_density_map_EXT = 0x01000000,

      // Provided by VK_KHR_fragment_shading_rate
      fragment_shading_rate_attachment_KHR = 0x40000000
    };
    enum class sparse_image_format {
      single_miptail = 0x00000001,
      aligned_mip_size = 0x00000002,
      nonstandard_block_size = 0x00000004,
    };
    enum class pipeline_creation_feedback {
      valid = 0x00000001,
      application_pipeline_cache_hit = 0x00000002,
      base_pipeline_acceleration = 0x00000004
    };

    enum class access {
      indirect_command_read = 0x00000001,
      index_read = 0x00000002,
      vertex_attribute_read = 0x00000004,
      uniform_read = 0x00000008,
      input_attachment_read = 0x00000010,
      shader_read = 0x00000020,
      shader_write = 0x00000040,
      color_attachment_read = 0x00000080,
      color_attachment_write = 0x00000100,
      depth_stencil_attachment_read = 0x00000200,
      depth_stencil_attachment_write = 0x00000400,
      transfer_read = 0x00000800,
      transfer_write = 0x00001000,
      host_read = 0x00002000,
      host_write = 0x00004000,
      memory_read = 0x00008000,
      memory_write = 0x00010000,

      // Provided by VK_EXT_transform_feedback
      transform_feedback_write_EXT = 0x02000000,
      transform_feedback_counter_read_EXT = 0x04000000,
      transform_feedback_counter_write_EXT = 0x08000000,

      // Provided by VK_EXT_conditional_rendering
      conditional_rendering_read_EXT = 0x00100000,
      color_attachment_read_noncoherent_EXT = 0x00080000,

      // Provided by VK_KHR_acceleration_structure
      acceleration_structure_read_KHR = 0x00200000,
      acceleration_structure_write_KHR = 0x00400000,

      // Provided by VK_NV_shading_rate_image
      shading_rate_image_read_NV = 0x00800000,

      // Provided by VK_EXT_fragment_density_map
      fragment_density_map_read_EXT = 0x01000000,

      // Provided by VK_NV_device_generated_commands
      command_preprocess_read_NV = 0x00020000,
      command_preprocess_write_NV = 0x00040000,

      // Provided by VK_NV_ray_tracing
      acceleration_structure_read_NV = acceleration_structure_read_KHR,
      acceleration_structure_write_NV = acceleration_structure_write_KHR,

      // Provided by VK_KHR_fragment_shading_rate
      fragment_shading_rate_attachment_read = shading_rate_image_read_NV
    };
    enum class pipeline_shader_stage {
      allow_varying_subgroup_size = 0x00000001,
      require_full_subgroups = 0x00000002,
    };
    enum class resolve_mode {
      sample_zero = 0x00000001,
      average = 0x00000002,
      min = 0x00000004,
      max = 0x00000008
    };
    enum class descriptor_binding {
      update_after_bind = 0x00000001,
      update_unused_while_pending = 0x00000002,
      partially_bound = 0x00000004,
      variable_descriptor_count = 0x00000008
    };

    enum class attachment_description {
      may_alias = 0x1
    };
    enum class dependency {
      by_region = 0x00000001,
      device_group = 0x00000004,
      view_local = 0x00000002,
    };

    enum class subpass_description {
      per_view_attributes_NVX = 0x00000001,
      per_view_position_x_only_NVX = 0x00000002,
      fragment_region_QCOM = 0x00000004,
      shader_resolve_QCOM = 0x00000008,
    };
    enum class command_pool_reset {
      release_resources = 0x1
    };
    enum class command_buffer_usage {
      one_time_submit = 0x00000001,
      render_pass_continue = 0x00000002,
      simultaneous_use = 0x00000004,
    };
    enum class command_buffer_reset {
      release_resources = 0x1
    };

    enum class query_pipeline_statistic {
      input_assembly_vertices = 0x00000001,
      input_assembly_primitives = 0x00000002,
      vertex_shader_invocations = 0x00000004,
      geometry_shader_invocations = 0x00000008,
      geometry_shader_primitives = 0x00000010,
      clipping_invocations = 0x00000020,
      clipping_primitives = 0x00000040,
      fragment_shader_invocations = 0x00000080,
      tessellation_control_shader_patches = 0x00000100,
      tessellation_evaluation_shader_invocations = 0x00000200,
      compute_shader_invocations = 0x00000400,
    };
    enum class query_result {
      x64 = 0x00000001,
      wait = 0x00000002,
      with_availability = 0x00000004,
      partial = 0x00000008,
      with_status_KHR = 0x00000010
    };
    enum class query_control {
      precise = 0x1
    };

    enum class indirect_state {
      frontface = 0x00000001
    };
    enum class indirect_commands_layout_usage {
      explicit_preprocess = 0x00000001,
      indexed_sequences   = 0x00000002,
      unordered_sequences = 0x00000004
    };

    enum class external_fence_feature {
      exportable = 0x1,
      importable = 0x2
    };
    enum class external_fence_handle_type {
      opaque_fd = 0x00000001,
      opaque_win32 = 0x00000002,
      opaque_win32_kmt = 0x00000004,
      sync_fd = 0x00000008
    };
    enum class fence_import {
      temporary = 0x1
    };

    enum class semaphore_wait {
      any = 0x1
    };

    enum class external_semaphore_feature {
      exportable = 0x1,
      importable = 0x2
    };
    enum class semaphore_import {
      temporary = 0x1
    };
    enum class external_semaphore_handle_type {
      opaque_fd = 0x00000001,
      opaque_win32 = 0x00000002,
      opaque_win32_kmt = 0x00000004,
      d3d12_fence = 0x00000008,
      sync_fd = 0x00000010,
      zircon_event_FUCHSIA = 0x00000080
    };

    enum class sparse_memory_bind {
      metadata = 0x1
    };

    enum class memory_heap {
      device_local = 0x00000001,
      multi_instance = 0x00000002,
    };
    enum class memory_property {
      device_local = 0x00000001,
      host_visible = 0x00000002,
      host_coherent = 0x00000004,
      host_cached = 0x00000008,
      lazily_allocated = 0x00000010,
      protected_memory = 0x00000020,
      device_coherent_AMD = 0x00000040,
      device_uncached_AMD = 0x00000080,
    };
    enum class peer_memory_feature {
      copy_src = 0x00000001,
      copy_dst = 0x00000002,
      generic_src = 0x00000004,
      generic_dst = 0x00000008
    };
    enum class memory_allocate {
      device_mask = 0x00000001,
      device_address = 0x00000002,
      device_address_capture_replay = 0x00000004
    };
    enum class external_memory_handle_type {
      opaque_fd                       = 0x00000001,
      opaque_win32                    = 0x00000002,
      opaque_win32_kmt                = 0x00000004,
      d3d11_texture                   = 0x00000008,
      d3d11_texture_kmt               = 0x00000010,
      d3d12_heap                      = 0x00000020,
      d3d12_resource                  = 0x00000040,
      dma_buf_EXT                     = 0x00000200,
      android_hardware_buffer_ANDROID = 0x00000400,
      host_allocation_EXT             = 0x00000080,
      host_mapped_foreign_memory_EXT  = 0x00000100,
      zircon_vmo_FUCHSIA              = 0x00000800
    };
    enum class external_memory_feature {
      dedicated_only = 0x00000001,
      exportable = 0x00000002,
      importable = 0x00000004,
    };

    enum class subgroup_feature {
      basic = 0x00000001,
      vote = 0x00000002,
      arithmetic = 0x00000004,
      ballot = 0x00000008,
      shuffle = 0x00000010,
      shuffle_relative = 0x00000020,
      clustered = 0x00000040,
      quad = 0x00000080,
      partitioned_NV = 0x00000100,
    };

    enum class debug_message_severity_EXT {
      verbose = 0x00000001,
      info = 0x00000010,
      warning = 0x00000100,
      error = 0x00001000
    };
    enum class debug_message_type_EXT {
      general = 0x00000001,
      validation = 0x00000002,
      performance = 0x00000004
    };
    enum class tool_purpose_EXT {
      validation = 0x00000001,
      profiling = 0x00000002,
      tracing = 0x00000004,
      additional_features = 0x00000008,
      modifying_features = 0x00000010,
      debug_reporting = 0x00000020,
      debug_markers = 0x00000040
    };
    enum class device_diagnostics_config_NV {
      enable_shader_debug_info = 0x00000001,
      enable_resource_tracking = 0x00000002,
      enable_automatic_checkpoints = 0x00000004
    };

    enum class geometry {
      opaque = 0x00000001,
      no_duplicate_any_hit_invocation = 0x00000002
    };
    enum class geometry_instance {
      triangle_facing_cull_disable = 0x00000001,
      triangle_front_counter_clockwise = 0x00000002,
      force_opaque = 0x00000004,
      force_no_opaque = 0x00000008
    };
    enum class build_acceleration_structure {
      allow_update = 0x00000001,
      allow_compaction = 0x00000002,
      prefer_fast_trace = 0x00000004,
      prefer_fast_build = 0x00000008,
      low_memory = 0x00000010
    };




    // VK_KHR_synchronization2

    enum class pipeline_stage2 : u64 {
      none = 0ull,
      top_of_pipe = 0x00000001ull,
      draw_indirect = 0x00000002ull,
      vertex_input = 0x00000004ull,
      vertex_shader = 0x00000008ull,
      tessellation_control_shader = 0x00000010ull,
      tessellation_evaluation_shader = 0x00000020ull,
      geometry_shader = 0x00000040ull,
      fragment_shader = 0x00000080ull,
      early_fragment_tests = 0x00000100ull,
      late_fragment_tests = 0x00000200ull,
      color_attachment_output = 0x00000400ull,
      compute_shader = 0x00000800ull,
      all_transfer = 0x00001000ull,
      transfer = 0x00001000,
      bottom_of_pipe = 0x00002000ull,
      host = 0x00004000ull,
      all_graphics = 0x00008000ull,
      all_commands = 0x00010000ull,
      copy = 0x100000000ull,
      resolve = 0x200000000ull,
      blit = 0x400000000ull,
      clear = 0x800000000ull,
      index_input = 0x1000000000ull,
      vertex_attribute_input = 0x2000000000ull,
      pre_rasterization_shaders = 0x4000000000ull,
      video_decode = 0x04000000ull,
      video_encode = 0x08000000ull,
      transform_feedback_EXT = 0x01000000ull,
      conditional_rendering_EXT = 0x00040000ull,
      command_preprocess_NV = 0x00020000ull,
      fragment_shading_rate_attachment = 0x00400000ull,
      shading_rate_image_NV = 0x00400000,
      acceleration_structure_build = 0x02000000ull,
      ray_tracing_shader = 0x00200000ull,
      fragment_density_process_EXT = 0x00800000ull,
      task_shader_NV = 0x00080000ull,
      mesh_shader_NV = 0x00100000ull
    };
    enum class access2 : u64 {
      none = 0ull,
      indirect_command_read = 0x00000001ull,
      index_read = 0x00000002ull,
      vertex_attribute_read = 0x00000004ull,
      uniform_read = 0x00000008ull,
      input_attachment_read = 0x00000010ull,
      shader_read = 0x00000020ull,
      shader_write = 0x00000040ull,
      color_attachment_read = 0x00000080ull,
      color_attachment_write = 0x00000100ull,
      depth_stencil_attachment_read = 0x00000200ull,
      depth_stencil_attachment_write = 0x00000400ull,
      transfer_read = 0x00000800ull,
      transfer_write = 0x00001000ull,
      host_read = 0x00002000ull,
      host_write = 0x00004000ull,
      memory_read = 0x00008000ull,
      memory_write = 0x00010000ull,
      shader_sampled_read = 0x100000000ull,
      shader_storage_read = 0x200000000ull,
      shader_storage_write = 0x400000000ull,
      video_decode_read = 0x800000000ull,
      video_decode_write = 0x1000000000ull,
      video_encode_read = 0x2000000000ull,
      video_encode_write = 0x4000000000ull,
      transform_feedback_write_EXT = 0x02000000ull,
      transform_feedback_counter_read_EXT = 0x04000000ull,
      transform_feedback_counter_write_EXT = 0x08000000ull,
      conditional_rendering_read_EXT = 0x00100000ull,
      command_preprocess_read_NV = 0x00020000ull,
      command_preprocess_write_NV = 0x00040000ull,
      fragment_shading_rate_attachment_read_KHR = 0x00800000ull,
      shading_rate_image_read_NV = 0x00800000,
      acceleration_structure_read_KHR = 0x00200000ull,
      acceleration_structure_write_KHR = 0x00400000ull,
      acceleration_structure_read_NV = 0x00200000,
      acceleration_structure_write_NV = 0x00400000,
      fragment_density_map_read_EXT = 0x01000000ull,
      color_attachment_read_noncoherent_EXT = 0x00080000ull
    };
    enum class submit : u32 {
      protected_memory = 1
    };

  }



  using descriptor_set_layout_flags = bitflags<flag_bits::descriptor_set_layout>;
  using pipeline_flags = bitflags<flag_bits::pipeline>;
  using queue_create_flags = bitflags<flag_bits::queue_create>;
  using command_pool_flags = bitflags<flag_bits::command_pool>;
  using swapchain_flags = bitflags<flag_bits::swapchain>;
  using image_flags = bitflags<flag_bits::image>;
  using image_view_flags = bitflags<flag_bits::image_view>;
  using buffer_flags = bitflags<flag_bits::buffer>;
  using framebuffer_flags = bitflags<flag_bits::framebuffer>;
  using pipeline_cache_flags = bitflags<flag_bits::pipeline_cache>;
  using sampler_flags = bitflags<flag_bits::sampler>;
  using descriptor_pool_flags = bitflags<flag_bits::descriptor_pool>;
  using render_pass_flags = bitflags<flag_bits::render_pass>;
  using conditional_rendering_flags = bitflags<flag_bits::conditional_rendering>;
  using fence_flags = bitflags<flag_bits::fence>;


  // Bitflags

  using cull_mode_flags = bitflags<flag_bits::cull_mode>;
  using stencil_face_flags = bitflags<flag_bits::stencil_face>;
  using color_component_flags = bitflags<flag_bits::color_component>;
  using sample_count_flags = bitflags<flag_bits::sample_count>;
  using surface_transform_flags = bitflags<flag_bits::surface_transform>;
  using composite_alpha_flags = bitflags<flag_bits::composite_alpha>;
  using device_group_present_mode_flags = bitflags<flag_bits::device_group_present_mode>;
  using pipeline_stage_flags = bitflags<flag_bits::pipeline_stage>;
  using shader_stage_flags = bitflags<flag_bits::shader_stage>;

  using image_usage_flags = bitflags<flag_bits::image_usage>;
  using image_aspect_flags = bitflags<flag_bits::image_aspect>;
  using buffer_usage_flags = bitflags<flag_bits::buffer_usage>;

  using device_queue_flags = bitflags<flag_bits::device_queue>;

  using performance_counter_flags = bitflags<flag_bits::performance_counter>;
  using format_feature_flags = bitflags<flag_bits::format_feature>;
  using sparse_image_format_flags = bitflags<flag_bits::sparse_image_format>;
  using pipeline_creation_feedback_flags = bitflags<flag_bits::pipeline_creation_feedback>;

  using access_flags = bitflags<flag_bits::access>;
  using pipeline_shader_stage_flags = bitflags<flag_bits::pipeline_shader_stage>;
  using resolve_mode_flags = bitflags<flag_bits::resolve_mode>;
  using descriptor_binding_flags = bitflags<flag_bits::descriptor_binding>;

  using attachment_description_flags = bitflags<flag_bits::attachment_description>;
  using dependency_flags = bitflags<flag_bits::dependency>;

  using subpass_description_flags = bitflags<flag_bits::subpass_description>;
  using command_pool_reset_flags = bitflags<flag_bits::command_pool_reset>;
  using command_buffer_usage_flags = bitflags<flag_bits::command_buffer_usage>;
  using command_buffer_reset_flags = bitflags<flag_bits::command_buffer_reset>;

  using query_pipeline_statistic_flags = bitflags<flag_bits::query_pipeline_statistic>;
  using query_result_flags = bitflags<flag_bits::query_result>;
  using query_control_flags = bitflags<flag_bits::query_control>;

  using indirect_state_flags = bitflags<flag_bits::indirect_state>;
  using indirect_commands_layout_usage_flags = bitflags<flag_bits::indirect_commands_layout_usage>;

  using external_fence_feature_flags = bitflags<flag_bits::external_fence_feature>;
  using external_fence_handle_type_flags = bitflags<flag_bits::external_fence_handle_type>;
  using fence_import_flags = bitflags<flag_bits::fence_import>;

  using semaphore_wait_flags = bitflags<flag_bits::semaphore_wait>;

  using external_semaphore_feature_flags = bitflags<flag_bits::external_semaphore_feature>;
  using semaphore_import_flags = bitflags<flag_bits::semaphore_import>;
  using external_semaphore_handle_type_flags = bitflags<flag_bits::external_semaphore_handle_type>;

  using sparse_memory_bind_flags = bitflags<flag_bits::sparse_memory_bind>;

  using memory_heap_flags = bitflags<flag_bits::memory_heap>;
  using memory_property_flags = bitflags<flag_bits::memory_property>;
  using peer_memory_feature_flags = bitflags<flag_bits::peer_memory_feature>;
  using memory_allocate_flags = bitflags<flag_bits::memory_allocate>;
  using external_memory_handle_type_flags = bitflags<flag_bits::external_memory_handle_type>;
  using external_memory_feature_flags = bitflags<flag_bits::external_memory_feature>;

  using subgroup_feature_flags = bitflags<flag_bits::subgroup_feature>;

  using debug_message_severity_flags_EXT = bitflags<flag_bits::debug_message_severity_EXT>;
  using debug_message_type_flags_EXT = bitflags<flag_bits::debug_message_type_EXT>;
  using tool_purpose_flags_EXT = bitflags<flag_bits::tool_purpose_EXT>;
  using device_diagnostics_config_flags_NV = bitflags<flag_bits::device_diagnostics_config_NV>;

  using geometry_flags = bitflags<flag_bits::geometry>;
  using geometry_instance_flags = bitflags<flag_bits::geometry_instance>;
  using build_acceleration_structure_flags = bitflags<flag_bits::build_acceleration_structure>;




  // VK_KHR_synchronization2

  using pipeline_stage2_flags = bitflags<flag_bits::pipeline_stage2>;
  using access2_flags = bitflags<flag_bits::access2>;
  using submit_flags = bitflags<flag_bits::submit>;




  /*namespace flags {
    struct pipeline;
    struct descriptor_set_layout;
    struct shader_stage;
    struct pipeline_stage;
    struct cull_mode;
    struct stencil_face;
    struct sample_count;


    struct swapchain;
    struct surface_transform;
    struct composite_alpha;
    struct present_mode;

    struct access;

    struct color_component;
    struct image_usage;
    struct buffer_usage;
    struct image_aspect;
    struct format_feature;

    struct queue;
    struct performance_counter;
    struct command_pool;
  }*/
}
#if 0
namespace valkyrie::traits{
  template <>
  struct BitFlagEnum<graphics::api::flag_bits::cull_mode>{
    using bit_type = graphics::api::flag_bits::cull_mode;
    using flag_type = graphics::api::cull_mode_flags;
    inline constexpr static string_view name{VK_string("cull_mode")};
    inline constexpr static string_view scoped_name{VK_string("valkyrie::graphics::api::flag_bits::cull_mode")};

  };
}




namespace valkyrie::graphics::api{
  

#define VK_declare_flag_type(Type) class Type##Flags : public CRTPBitFlags<Type##Flags, flag_bits::Type>{ \
    using Base_ = CRTPBitFlags<Type##Flags, flag_bits::Type>;                                             \
  public:                                                                                                      \
    using Base_::Base_;                                                                                        \
    using Bit = Flags::Type;                                                                                   \
    constexpr Type##Flags() = default;                                                                         \
  }

#define PP_VK_impl_INLINE_FLAG_VALUE(Type, Flag) inline constexpr static Type##Flags Flag{ flag_bits::Type::Flag };
#define VK_inline_flags(Type, ...) VK_foreach(VK_bind_front(PP_VK_impl_INLINE_FLAG_VALUE, Type), __VA_ARGS__)
  
  
  VK_declare_flag_type(cull_mode);
  VK_declare_flag_type(stencil_face);
  VK_declare_flag_type(sample_count);
  VK_declare_flag_type(color_component);
  VK_declare_flag_type(shader_stage);
  VK_declare_flag_type(pipeline_stage);

  VK_declare_flag_type(surface_transform);
  VK_declare_flag_type(composite_alpha);
  VK_declare_flag_type(present_mode);
  
  VK_declare_flag_type(access);
  
  VK_declare_flag_type(image_usage);
  VK_declare_flag_type(image_aspect);
  VK_declare_flag_type(buffer_usage);
  VK_declare_flag_type(format_feature);
  

  VK_declare_flag_type(descriptor_set_layout);
  VK_declare_flag_type(pipeline);
  VK_declare_flag_type(queue);
  VK_declare_flag_type(performance_counter);
  VK_declare_flag_type(command_pool);
  VK_declare_flag_type(swapchain);
  
  namespace flags{
    
    // = 0x[01248']+
    struct cull_mode{
      cull_mode() = delete;

      VK_inline_flags(cull_mode,
                      none,
                      front,
                      back,
                      front_and_back)
    };

    struct stencil_face{
      VK_inline_flags(stencil_face,
                      front,
                      back,
                      front_and_back)
    };



    struct sample_count{
      VK_inline_flags(sample_count,
                      x1,
                      x2,
                      x4,
                      x8,
                      x16,
                      x32,
                      x64)
    };
    struct shader_stage{
      VK_inline_flags(shader_stage,
                      vertex,
                      tessellation_control,
                      tessellation_evaluation,
                      geometry,
                      fragment,
                      compute,
                      task,
                      mesh,
                      ray_gen,
                      any_hit,
                      closest_hit,
                      miss,
                      intersection,
                      callable)
    };
    struct color_component{
      VK_inline_flags(color_component,
                      r,
                      g,
                      b,
                      a);
    };
    struct pipeline_stage{
      VK_inline_flags(pipeline_stage,
                      top_of_pipe,
                      draw_indirect,
                      vertex_input,
                      vertex_shader,
                      tessellation_control_shader,
                      tessellation_evaluation_shader,
                      geometry_shader,
                      fragment_shader,
                      early_fragment_tests,
                      late_fragment_tests,
                      color_attachment_output,
                      compute_shader,
                      transfer,
                      bottom_of_pipe,
                      host,
                      all_graphics,
                      all_commands,
                      transform_feedback,
                      conditional_rendering,
                      command_process,
                      ray_tracing_shader,
                      acceleration_structure_build,
                      task_shader,
                      mesh_shader,
                      fragment_density_process);
    };
    
    
    struct surface_transform{
      VK_inline_flags(surface_transform,
                      identity,
                      rotate90,
                      rotate180,
                      rotate270,
                      horizontal_mirror,
                      horizontal_mirror_rotate90,
                      horizontal_mirror_rotate180,
                      horizontal_mirror_rotate270,
                      inherit);
    };
    struct composite_alpha{
      VK_inline_flags(composite_alpha,
                      opaque,
                      pre_multiplied,
                      post_multiplied,
                      inherit)
    };
    struct present_mode{
      VK_inline_flags(present_mode,
                      local,
                      remote,
                      sum,
                      local_multi_device)
    };
    
    struct access{
      VK_inline_flags(access,
                      indirect_command_read,
                      index_read,
                      vertex_attribute_read,
                      uniform_read,
                      input_attachment_read,
                      shader_read,
                      shader_write,
                      color_attachment_read,
                      color_attachment_write,
                      depth_stencil_attachment_read,
                      depth_stencil_attachment_write,
                      transfer_read,
                      transfer_write,
                      host_read,
                      host_write,
                      memory_read,
                      memory_write,
                      transform_feedback_write_EXT,
                      transform_feedback_counter_read_EXT,
                      transform_feedback_counter_write_EXT,
                      conditional_rendering_read_EXT,
                      color_attachment_read_noncoherent_EXT,
                      acceleration_structure_read_KHR,
                      acceleration_structure_write_KHR,
                      shading_rate_image_read_NV,
                      fragment_density_map_read_EXT,
                      command_preprocess_read_NV,
                      command_preprocess_write_NV)
    };

    struct image_aspect{
      VK_inline_flags(image_aspect,
                      color,
                      depth,
                      stencil,
                      metadata,
                      plane_0,
                      plane_1,
                      plane_2,
                      memory_plane_0,
                      memory_plane_1,
                      memory_plane_2,
                      memory_plane_3)
    };
    struct image_usage{
      VK_inline_flags(image_usage,
                      transfer_src,
                      transfer_dst,
                      sampled    ,
                      storage    ,
                      color_attachment,
                      depth_stencil_attachment,
                      transient_attachment,
                      input_attachment,
                      shading_rate_image,
                      fragment_density_map)
    };
    struct buffer_usage{
      VK_inline_flags(buffer_usage,
                      transfer_bit,
                      transfer_dst,
                      uniform_texel_buffer,
                      storage_texel_buffer,
                      uniform_buffer,
                      storage_buffer,
                      index_buffer,
                      vertex_buffer,
                      indirect_buffer,
                      shader_device_address,
                      transform_feedback_buffer,
                      transform_feedback_counter_buffer,
                      conditional_rendering,
                      ray_tracing)
    };
    struct format_feature{
      VK_inline_flags(format_feature,
                      sampled_image,
                      storage_image,
                      storage_image_atomic,
                      uniform_texel_buffer,
                      storage_texel_buffer,
                      storage_texel_buffer_atomic,
                      vertex_buffer,
                      color_attachment,
                      color_attachment_blend,
                      depth_stencil_attachment,
                      blit_src,
                      blit_dst,
                      sampled_image_filter_linear,
                      transfer_src,
                      transfer_dst,
                      midpoint_chroma_samples,
                      sampled_image_ycbcr_conversion_linear_filter,
                      sampled_image_ycbcr_conversion_separate_reconstruction_filter,
                      sampled_image_ycbcr_conversion_chroma_reconstruction_explicit,
                      sampled_image_ycbcr_conversion_chroma_reconstruction_explicit_forceable,
                      disjoint,
                      cosited_chroma_samples,
                      sampled_image_filter_minmax,
                      sampled_image_filter_cubic_IMG,
                      acceleration_structure_vertex_buffer_KHR,
                      fragment_density_map_EXT,
                      fragment_shading_rate_attachment_KHR)
    };

    struct descriptor_set_layout {
      VK_inline_flags(descriptor_set_layout,
                      update_after_bind_pool,
                      push_descriptor)
    };
    struct pipeline {
      VK_inline_flags(pipeline,
                      disable_optimization,
                      allow_derivatives,
                      derivative,
                      view_index_from_device_index,
                      dispatch_base,
                      ray_tracing_no_null_any_hit_shaders,
                      ray_tracing_no_null_closest_hit_shaders,
                      ray_tracing_no_null_miss_shaders,
                      ray_tracing_no_null_intersection_shaders,
                      ray_tracing_skip_triangles,
                      RayTracingSkipAABBs,
                      defer_compile,
                      capture_statistics,
                      capture_internal_representations,
                      indirect_bindable,
                      library,
                      fail_on_pipeline_compile_required,
                      early_return_on_failure)
    };
    struct queue {
      VK_inline_flags(queue,
                      graphics,
                      compute,
                      transfer,
                      sparse_binding,
                      protected_memory)
    };
    struct performance_counter {
      VK_inline_flags(performance_counter,
                      performance_impacting,
                      concurrently_impacted)
    };
    struct command_pool {
      VK_inline_flags(command_pool,
                      transient,
                      reset_command_buffer,
                      protected_memory)
    };
    struct swapchain {
      VK_inline_flags(swapchain,
                      split_instance_bind_regions,
                      protected_memory,
                      mutable_format)
    };
  }

#undef VK_inline_flags
#undef VK_declare_flag_type

}
#endif


#endif//VALKYRIE_VULKAN_ENUMS_HPP
