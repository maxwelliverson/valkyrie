//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_GRAPHICS_API_QUEUE_HPP
#define VALKYRIE_GRAPHICS_API_QUEUE_HPP

// #include <valkyrie/adt/array_ref.hpp>

#include "vulkan.hpp"


namespace valkyrie::graphics::api{

  VK_declare_struct(device_queue_create_info);
  VK_declare_struct(device_queue_global_priority_create_info_EXT);

  VK_define_in_struct(device_queue_create_info){


    device_queue_create_info(u32 queueFamily, std::span<const f32> queuePriorities, queue_create_flags flags = {}) noexcept
        : flags(flags),
          queueFamilyIndex(queueFamily),
          queueCount(queuePriorities.size()),
          pQueuePriorities(queuePriorities.data()){}

    device_queue_create_info& set_flags(queue_create_flags queueFlags) noexcept {
      flags = queueFlags;
      return *this;
    }
    device_queue_create_info& set_queue_family(u32 queueFamily) noexcept {
      queueFamilyIndex = queueFamily;
      return *this;
    }
    device_queue_create_info& set_queue_priorities(std::span<const f32> queuePriorities) noexcept {
      queueCount       = queuePriorities.size();
      pQueuePriorities = queuePriorities.data();
      return *this;
    }

  private:
    queue_create_flags flags{};
    u32                queueFamilyIndex;
    u32                queueCount;
    const f32*         pQueuePriorities;
  };

  VK_define_in_struct(device_queue_global_priority_create_info_EXT, device_queue_create_info){

    device_queue_global_priority_create_info_EXT() = default;
    explicit device_queue_global_priority_create_info_EXT(queue_global_priority_EXT globalPriority) noexcept
        : queueGlobalPriority(globalPriority){ }

    device_queue_global_priority_create_info_EXT& set_global_priority(queue_global_priority_EXT globalPriority) noexcept {
      queueGlobalPriority = globalPriority;
      return *this;
    }

  private:
    queue_global_priority_EXT queueGlobalPriority;
  };


  /*enum class Priority{
    Low,
    Default,
    High,
    Realtime
  };


  class queue : public vulkan_object{
  protected:
    device * pDevice;
  };
  class graphics_queue : public queue {};
  class compute_queue : public queue {};
  class transfer_queue : public queue {};
  class sparse_binding_queue : public queue {};*/


  /*class device_queue_global_priority : public in_structure{
  private:
    queue_global_priority level;
  public:
    constexpr device_queue_global_priority(queue_global_priority level) noexcept
        : in_structure(structure_type::device_queue_global_priority_create_info_EXT),
          level(level){}

    constexpr device_queue_global_priority& set_priority(queue_global_priority level) noexcept {
      this->level = level;
      return *this;
    }
  };
  class device_queue_create_info : public in_structure{
    const u32 flags = 0; // Protected memory...
    u32 queueFamilyIndex;
    u32 queueCount = 0;
    const float* pQueuePriorities = nullptr;
  public:
    constexpr device_queue_create_info(u32 queueFamily, span<const float> priorities = {}) noexcept
        : in_structure(structure_type::device_queue_create_info),
          queueFamilyIndex(queueFamily),
          queueCount(priorities.size()),
          pQueuePriorities(priorities.data()){}

    constexpr device_queue_create_info& set_priorities(span<const float> priorities) noexcept {
      queueCount = priorities.size();
      pQueuePriorities = priorities.data();
      return *this;
    }
  };*/


}

#endif//VALKYRIE_GRAPHICS_API_QUEUE_HPP
