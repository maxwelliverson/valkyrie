//
// Created by maxwe on 2021-04-02.
//

#ifndef VALKYRIE_GRAPHICS_ENGINE_RESOURCE_HPP
#define VALKYRIE_GRAPHICS_ENGINE_RESOURCE_HPP


#include <valkyrie/traits.hpp>
#include <valkyrie/adt/small_vector.hpp>
#include <valkyrie/async/atomic.hpp>
#include <vulkan/vulkan_core.h>

namespace valkyrie::graphics::engine{

  class physical_device{
  public:



  private:
    uuid  uuid_;
    void* vulkan_handle_;
    i32   cuda_handle_;
    i32   cuda_ordinal_;
  };

  class device{
  public:
  private:
    void*                         vulkan_handle_;
    small_vector<physical_device> physical_devices_;
  };


  class resource_lifetime{ };
  class resource_scope{ };
  class resource_memory{ };


  class device_allocator;

  class device_memory{
    using handle_t = void*;
    using native_handle_t = VK_if(VK_system_windows(void*)VK_else(int));
  public:



  private:
    handle_t        handle_;
    native_handle_t native_handle_;
    atomic<u64>     ref_count_;
  };

  class device_allocation{
  public:
  private:
    device_memory* memory_block_;
    u64            size_;
    u64            offset_;
  };

  class device_allocator{
  public:
    virtual ~device_allocator() = default;


    virtual device*        do_get_device() const noexcept = 0;

    virtual device_memory* do_allocate() noexcept = 0;
    virtual device_memory* do_reallocate(device_memory* pDeviceMemory) noexcept = 0;
    virtual void           do_deallocate(device_memory* deviceMemory) noexcept = 0;
  };



  class resource{
  public:
    virtual ~resource() = default;

    virtual bool is_dedicated() const noexcept;
  };
  class resource_view{
  public:
    virtual ~resource_view() = default;

    virtual resource* get_backing_resource() noexcept = 0;
  };

  class image : public resource{ };
  class buffer : public resource{ };


  class image_view : public resource_view{
  public:
    image* get_backing_resource() noexcept override = 0;
  };
  class buffer_view : public resource_view{
  public:
    buffer* get_backing_resource() noexcept override = 0;
  };
}

#endif//VALKYRIE_GRAPHICS_ENGINE_RESOURCE_HPP
