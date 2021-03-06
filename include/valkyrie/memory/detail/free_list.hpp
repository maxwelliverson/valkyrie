//
// Created by maxwe on 2021-03-05.
//

#ifndef VALKYRIE_MEMORY_DETAIL_FREE_LIST_HPP
#define VALKYRIE_MEMORY_DETAIL_FREE_LIST_HPP

namespace valkyrie::detail{
  // stores free blocks for a memory pool
  // memory blocks are fragmented and stored in a list
  // debug: fills memory and uses a bigger node_size for fence memory
  class free_memory_list
  {
  public:
    // minimum element size
    static constexpr auto min_element_size = sizeof(char*);
    // alignment
    static constexpr auto min_element_alignment = alignof(char*);

    //=== constructor ===//
    free_memory_list(u64 node_size) noexcept;

    // calls other constructor plus insert
    free_memory_list(u64 node_size, void* mem,
                     u64 size) noexcept;

    free_memory_list(free_memory_list&& other) noexcept;
    ~free_memory_list() noexcept = default;

    free_memory_list& operator=(free_memory_list&& other) noexcept;

    friend void swap(free_memory_list& a, free_memory_list& b) noexcept;

    //=== insert/allocation/deallocation ===//
    // inserts a new memory block, by splitting it up and setting the links
    // does not own memory!
    // mem must be aligned for alignment()
    // pre: size != 0
    void insert(void* mem, u64 size) noexcept;

    // returns the usable size
    // i.e. how many memory will be actually inserted and usable on a call to insert()
    u64 usable_size(u64 size) const noexcept
    {
      return size;
    }

    // returns a single block from the list
    // pre: !empty()
    void* allocate() noexcept;

    // returns a memory block big enough for n bytes
    // might fail even if capacity is sufficient
    void* allocate(u64 n) noexcept;

    // deallocates a single block
    void deallocate(void* ptr) noexcept;

    // deallocates multiple blocks with n bytes total
    void deallocate(void* ptr, u64 n) noexcept;

    //=== getter ===//
    u64 node_size() const noexcept
    {
      return node_size_;
    }

    // alignment of all nodes
    u64 alignment() const noexcept;

    // number of nodes remaining
    u64 capacity() const noexcept
    {
      return capacity_;
    }

    bool empty() const noexcept
    {
      return first_ == nullptr;
    }

  private:
    u64 fence_size() const noexcept;
    void        insert_impl(void* mem, u64 size) noexcept;

    char*       first_;
    u64 node_size_, capacity_;
  };

  void swap(free_memory_list& a, free_memory_list& b) noexcept;

  // same as above but keeps the nodes ordered
  // this allows array allocations, that is, consecutive nodes
  // debug: fills memory and uses a bigger node_size for fence memory
  class ordered_free_memory_list
  {
  public:
    // minimum element size
    static constexpr auto min_element_size = sizeof(char*);
    // alignment
    static constexpr auto min_element_alignment = alignof(char*);

    //=== constructor ===//
    ordered_free_memory_list(u64 node_size) noexcept;

    ordered_free_memory_list(u64 node_size, void* mem,
                             u64 size) noexcept
        : ordered_free_memory_list(node_size)
        {
            insert(mem, size);
        }

    ordered_free_memory_list(ordered_free_memory_list&& other) noexcept;

    ~ordered_free_memory_list() noexcept = default;

    ordered_free_memory_list& operator=(ordered_free_memory_list&& other)
    noexcept
    {
      ordered_free_memory_list tmp(std::move(other));
      swap(*this, tmp);
      return *this;
    }

    friend void swap(ordered_free_memory_list& a,
                     ordered_free_memory_list& b) noexcept;

    //=== insert/allocation/deallocation ===//
    // inserts a new memory block, by splitting it up and setting the links
    // does not own memory!
    // mem must be aligned for alignment()
    // pre: size != 0
    void insert(void* mem, u64 size) noexcept;

    // returns the usable size
    // i.e. how many memory will be actually inserted and usable on a call to insert()
    u64 usable_size(u64 size) const noexcept
    {
      return size;
    }

    // returns a single block from the list
    // pre: !empty()
    void* allocate() noexcept;

    // returns a memory block big enough for n bytes (!, not nodes)
    // might fail even if capacity is sufficient
    void* allocate(u64 n) noexcept;

    // deallocates a single block
    void deallocate(void* ptr) noexcept;

    // deallocates multiple blocks with n bytes total
    void deallocate(void* ptr, u64 n) noexcept;

    //=== getter ===//
    u64 node_size() const noexcept
    {
      return node_size_;
    }

    // alignment of all nodes
    u64 alignment() const noexcept;

    // number of nodes remaining
    u64 capacity() const noexcept
    {
      return capacity_;
    }

    bool empty() const noexcept
    {
      return capacity_ == 0u;
    }

  private:
    u64 fence_size() const noexcept;

    // returns previous pointer
    char* insert_impl(void* mem, u64 size) noexcept;

    char* begin_node() noexcept;
    char* end_node() noexcept;

    std::uintptr_t begin_proxy_, end_proxy_;
    u64    node_size_, capacity_;
    char *         last_dealloc_, *last_dealloc_prev_;
  };

  void swap(ordered_free_memory_list& a, ordered_free_memory_list& b) noexcept;

#if VK_debug_build
  // use ordered version to allow pointer check
            using node_free_memory_list  = ordered_free_memory_list;
            using array_free_memory_list = ordered_free_memory_list;
#else
  using node_free_memory_list  = free_memory_list;
  using array_free_memory_list = ordered_free_memory_list;
#endif
}

#endif//VALKYRIE_MEMORY_DETAIL_FREE_LIST_HPP
