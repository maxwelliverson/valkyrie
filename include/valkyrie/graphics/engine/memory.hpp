//
// Created by maxwe on 2021-04-02.
//

#ifndef VALKYRIE_GRAPHICS_ENGINE_MEMORY_HPP
#define VALKYRIE_GRAPHICS_ENGINE_MEMORY_HPP

namespace valkyrie{



  class device_allocator;

  class device_memory{
    using handle_t = void*;
    using native_handle_t = VK_if(VK_system_windows(void*)VK_else(int));
  public:



  private:
    handle_t        handle_;
    native_handle_t native_handle_;
    u64             size;
    atomic<u64>     ref_count_;
  };

  class device_allocation{
  public:
  private:
    device_memory* memory_block_;
    u64            size_;
    u64            offset_;
    atomic<u32>    mapCount;
  };

  class device_lowlevel_allocator{
  protected:

    virtual u64            do_get_minimum_alignment() const noexcept { return max_alignment; }

    virtual device_memory* do_create(u64 size, u64 align) noexcept = 0;
    virtual void           do_destroy(device_memory* memory) noexcept = 0;



    virtual void*          do_map_device(device_memory* memory, device* dev, void* initialAddress, u64 mapSize, u64 mapOffset) noexcept = 0;
    virtual void*          do_map_host(device_memory* memory, void* initialAddress, u64 mapSize, u64 mapOffset) noexcept = 0;

    virtual void           do_unmap_device(device_memory* memory, device* dev, void* address, u64 size, u64 offset) noexcept = 0;
    virtual void           do_unmap_host(device_memory* memory, void* address, u64 size, u64 offset) noexcept = 0;



    virtual void           on_memory_create(device_memory* memory) noexcept { }
    virtual void           on_memory_destroy(device_memory* memory) noexcept { }
    virtual void           on_memory_map_device(device_memory* memory, device* dev, void* address, void* requestedAddress, u64 mapSize, u64 mapOffset) noexcept { }
    virtual void           on_memory_map_host(device_memory* memory, void* address, void* requestedAddress, u64 mapSize, u64 mapOffset) noexcept { }
    virtual void           on_memory_unmap_device(device_memory* memory, device* dev, void* address, u64 mapSize, u64 mapOffset) noexcept { }
    virtual void           on_memory_unmap_host(device_memory* memory, void* address, u64 mapSize, u64 mapOffset) noexcept { }

  public:
    virtual ~device_lowlevel_allocator() = default;

    device_memory* create_memory(u64 size) noexcept {
      return this->create_memory(size, this->do_get_minimum_alignment());
    }
    device_memory* create_memory(u64 size, u64 align) noexcept {
      auto* result = this->do_create(size, align);
      this->on_memory_create(result);
      return result;
    }
    void           destroy_memory(device_memory* memory) noexcept {
      this->on_memory_destroy(memory);
      this->do_destroy(memory);
    }

    void*          map_memory(const device_allocation& alloc) noexcept {

    }
    void*          map_memory(const device_allocation& alloc, device* dev) noexcept {

    }
    void*          map_memory(const device_allocation& alloc, void* desiredAddress) noexcept {

    }
    void*          map_memory(const device_allocation& alloc, void* desiredAddress, device* dev) noexcept {

    }
  };

  class device_allocator{
  protected:
    virtual device*                    do_get_device() const noexcept = 0;
    virtual device_lowlevel_allocator* do_get_lowlevel_allocator() const noexcept = 0;

    virtual device_allocation do_allocate(u64 size, u64 align, i64 offset) noexcept = 0;
    virtual device_allocation do_reallocate(const device_allocation& allocation, u64 size, u64 align, i64 offset) noexcept = 0;
    virtual void              do_deallocate(const device_allocation& allocation) noexcept = 0;

    virtual void              on_memory_allocate(const device_allocation& allocation) noexcept { }
    virtual void              on_memory_reallocate(const device_allocation& newAlloc, const device_allocation& oldAlloc) noexcept { }
    virtual void              on_memory_deallocate(const device_allocation& allocation) noexcept { }
  public:
    virtual ~device_allocator() = default;




  };
}

#endif//VALKYRIE_GRAPHICS_ENGINE_MEMORY_HPP
