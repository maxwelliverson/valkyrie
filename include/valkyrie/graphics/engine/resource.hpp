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
